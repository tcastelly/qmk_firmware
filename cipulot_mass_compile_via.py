import os
import sys
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
import json

# Set environment variables before importing qmk
CWD = os.getcwd()
os.environ["QMK_HOME"] = os.environ["ORIG_CWD"] = CWD

QMK_PY = Path("lib") / "python"
sys.path.append(str(QMK_PY))

from qmk.keyboard import list_keyboards
from qmk.build_targets import KeyboardKeymapBuildTarget

# Compatibility shim: some environments (or shadowed 'ast' modules) may not expose
# the legacy AST node classes like `ast.Num`. The `qmk` package (lib/python/qmk/math.py)
# still uses `ast.Num`, so ensure a minimal-compatible `ast.Num` exists before
# importing any qmk modules.
import ast

if not hasattr(ast, "Num"):
    # On newer Python ASTs numeric literals are `ast.Constant` instances.
    # Two things are needed so older code using `ast.Num` keeps working:
    # 1) `isinstance(node, ast.Num)` must succeed for Constant nodes, and
    # 2) `node.n` must return the numeric value (older code expects `.n`).
    # Achieve this by adding a `.n` property to ast.Constant and aliasing
    # ast.Num to ast.Constant.
    try:
        # add property 'n' to ast.Constant so attribute access works
        if not hasattr(ast.Constant, "n"):
            ast.Constant.n = property(lambda self: self.value)
    except Exception:
        # If ast.Constant is not assignable (shouldn't happen), fall back to
        # creating a thin subclass and aliasing (less ideal because existing
        # Constant instances won't be instances of the subclass).
        class _CompatNum(ast.Constant):
            @property
            def n(self):
                return self.value

        ast.Num = _CompatNum
    else:
        # Make ast.Num point to ast.Constant so isinstance checks succeed.
        ast.Num = ast.Constant


CWD = os.getcwd()
USERSPACE_VIA = os.path.abspath(os.path.join(CWD, "..", "qmk_userspace_via"))

everything = list_keyboards()
cipulot = list(filter(lambda x: x.startswith("cipulot"), list_keyboards()))

# remove previous artifacts
sys.stdout.write("Removing previous artifacts in QMK Firmware folder...\n")
CWD = Path(CWD)
for ext in ("uf2", "bin", "hex"):
    for file in CWD.glob(f"*.{ext}"):
        file.unlink()

# remove previous artifacts
sys.stdout.write("Removing previous artifacts in QMK Userspace VIA folder...\n")
USERSPACE_VIA = Path(USERSPACE_VIA)
for ext in ("uf2", "bin", "hex"):
    for file in USERSPACE_VIA.glob(f"*.{ext}"):
        file.unlink()


def find_all_definition_files(board_path):
    # Recursively find all keyboard.json and info.json files under board_path
    return list(board_path.rglob("keyboard.json")) + list(board_path.rglob("info.json"))


def compile_variant(def_file, userspace_via, log_lock):
    # Determine the board name from the path (relative to keyboards/)
    rel_path = def_file.relative_to(Path("keyboards"))
    board = str(rel_path.parent)
    # Load JSON
    with open(def_file, "r") as f:
        try:
            kb_data = json.load(f)
            processor = kb_data.get("processor", "")
        except Exception:
            processor = ""

    def do_compile(processor_override=None, alt_suffix=None):
        original_json = None
        if processor_override:
            with open(def_file, "r") as f:
                original_json = f.read()
            kb_data2 = json.loads(original_json)
            kb_data2["processor"] = processor_override
            with open(def_file, "w") as f:
                json.dump(kb_data2, f, indent=4)
        target = KeyboardKeymapBuildTarget(board, "via")
        target.configure(parallel=16)
        keymap_directory = Path(userspace_via) / "keyboards" / board / "keymaps" / "via"
        target.extra_args = {
            "MAIN_KEYMAP_PATH_1": str(keymap_directory),
            "MAIN_KEYMAP_PATH_2": str(keymap_directory),
            "MAIN_KEYMAP_PATH_3": str(keymap_directory),
            "MAIN_KEYMAP_PATH_4": str(keymap_directory),
            "MAIN_KEYMAP_PATH_5": str(keymap_directory),
        }
        compile_result = target.compile()
        proc = processor_override if processor_override else processor
        prefix_map = {
            "STM32F401": "F401_",
            "STM32F411": "F411_",
            "STM32F072": "F072_",
            "STM32G431": "G431_",
        }
        prefix = prefix_map.get(proc, None)
        bin_file = None
        for f in Path().glob("cipulot_*.bin"):
            if f.is_file():
                bin_file = f
                break
        if prefix and bin_file:
            new_name = bin_file.name.replace("cipulot_", prefix, 1)
            if alt_suffix:
                parts = new_name.rsplit(".", 1)
                new_name = parts[0] + alt_suffix + "." + parts[1]
            bin_file.rename(new_name)
        if processor_override and original_json is not None:
            with open(def_file, "w") as f:
                f.write(original_json)
        return compile_result

    try:
        compile_result = do_compile()
        compile_message = f"Compiling: {board} "
        dots = "." * (50 - len(compile_message))
        compile_message += dots
        alt_result = None
        alt_message = ""
        if processor == "STM32F401":
            alt_result = do_compile("STM32F411")
            alt_message = f" (also compiled as STM32F411)"
        elif processor == "STM32F411":
            alt_result = do_compile("STM32F401")
            alt_message = f" (also compiled as STM32F401)"
        if compile_result is not None:
            return (board, compile_message + alt_message, "FAILED!")
        else:
            return (board, compile_message + alt_message, "SUCCESS!")
    except Exception as e:
        compile_message = f"Compiling: {board} "
        dots = "." * (50 - len(compile_message))
        compile_message += dots
        return (board, compile_message, f"ERROR: {str(e)}")


sys.stdout.write("Compiling Cipulot boards with VIA...\n")

# Recursively find all keyboard.json and info.json files for all boards
boards_root = Path("keyboards/cipulot")
definition_files = []
for board in cipulot:
    board_path = boards_root / Path(board).relative_to("cipulot")
    definition_files.extend(find_all_definition_files(board_path))

log_lock = threading.Lock()
results = []
max_parallel_builds = 8
with ThreadPoolExecutor(max_workers=max_parallel_builds) as executor:
    future_to_def = {
        executor.submit(compile_variant, def_file, USERSPACE_VIA, log_lock): str(
            def_file
        )
        for def_file in definition_files
    }
    with (CWD / "cipulot.log").open("w") as file:
        for future in as_completed(future_to_def):
            board, compile_message, result = future.result()
            file.write(f"{compile_message} {result}\n")
            file.flush()
            sys.stdout.write(f"{compile_message} {result}\n")
            sys.stdout.flush()

import os
import json
import re


def parse_config_h(config_path):
    with open(config_path, "r") as file:
        content = file.read()

    # Extract UNUSED_POSITIONS_LIST
    unused_positions_match = re.search(
        r"#define UNUSED_POSITIONS_LIST \{(.*?)\}", content, re.DOTALL
    )
    if unused_positions_match:
        unused_positions_str = unused_positions_match.group(1)
        # Extract positions using regex
        unused_positions = re.findall(r"\{(\d+),\s*(\d+)\}", unused_positions_str)
        # Convert to list of tuples
        unused_positions = [(int(r), int(c)) for r, c in unused_positions]
    else:
        unused_positions = []

    # Extract matrix_rows and matrix_cols
    matrix_rows_match = re.search(r"#define MATRIX_ROWS (\d+)", content)
    matrix_cols_match = re.search(r"#define MATRIX_COLS (\d+)", content)

    if not matrix_rows_match or not matrix_cols_match:
        raise ValueError("MATRIX_ROWS or MATRIX_COLS not found in config.h")

    matrix_rows = int(matrix_rows_match.group(1))
    matrix_cols = int(matrix_cols_match.group(1))

    return unused_positions, matrix_rows, matrix_cols


def find_missing_pairs(layout, unused_positions, matrix_rows, matrix_cols):
    all_positions = {(r, c) for r in range(matrix_rows) for c in range(matrix_cols)}
    used_positions = set(tuple(pos["matrix"]) for pos in layout)
    unused_positions_set = set(unused_positions)

    missing_pairs = all_positions - used_positions - unused_positions_set
    return sorted(missing_pairs)


def format_pairs(pairs):
    return ", ".join(f"{{{r}, {c}}}" for r, c in pairs)


def sort_log_file(log_file_path):
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()

    no_config_lines = []
    no_missing_pairs_lines = []
    already_defined_lines = []
    other_lines = []

    for line in lines:
        if "MATRIX_ROWS or MATRIX_COLS not found in config.h" in line:
            no_config_lines.append(line)
        elif "NO MISSING PAIRS" in line:
            no_missing_pairs_lines.append(line)
        elif "UNUSED_POSITIONS_LIST already defined in config.h" in line:
            already_defined_lines.append(line)
        else:
            other_lines.append(line)

    sorted_lines = (
        sorted(no_config_lines)
        + sorted(no_missing_pairs_lines)
        + sorted(already_defined_lines)
        + sorted(other_lines)
    )

    with open(log_file_path, "w") as log_file:
        log_file.writelines(sorted_lines)


def main():
    base_path = "keyboards/cipulot"
    log_file_path = "cipulot.log"

    # Clear the log file before executing
    with open(log_file_path, "w") as log_file:
        log_file.write("")

    with open(log_file_path, "a") as log_file:
        for root, _, files in os.walk(base_path):
            if "keyboard.json" in files and "config.h" in files:
                keyboard_json_path = os.path.join(root, "keyboard.json")
                config_h_path = os.path.join(root, "config.h")

                try:
                    with open(keyboard_json_path, "r") as file:
                        keyboard_data = json.load(file)
                except json.JSONDecodeError:
                    log_file.write(f"Error decoding JSON in {keyboard_json_path}\n")
                    continue

                board_name = keyboard_data.get("keyboard_name", "Unknown Board")

                layouts = keyboard_data.get("layouts", {})
                layout = None
                layout_name = None

                # Check for LAYOUT_all or LAYOUT
                for key in ["LAYOUT_all", "LAYOUT"]:
                    if key in layouts:
                        layout = layouts[key]["layout"]
                        layout_name = key
                        break

                # If neither LAYOUT_all nor LAYOUT is found, find the layout with the highest number of elements
                if not layout:
                    max_elements = 0
                    for key, value in layouts.items():
                        if len(value["layout"]) > max_elements:
                            max_elements = len(value["layout"])
                            layout = value["layout"]
                            layout_name = key

                if not layout:
                    log_file.write(f"No suitable layout found for {board_name}\n")
                    continue

                try:
                    unused_positions, matrix_rows, matrix_cols = parse_config_h(
                        config_h_path
                    )
                except ValueError as e:
                    log_file.write(f"{board_name}: {str(e)}\n")
                    continue

                missing_pairs = find_missing_pairs(
                    layout, unused_positions, matrix_rows, matrix_cols
                )

                if not missing_pairs:
                    log_file.write(f"{board_name} ({layout_name}): NO MISSING PAIRS\n")
                else:
                    formatted_pairs = format_pairs(missing_pairs)
                    # Normalize and compare UNUSED_POSITIONS_LIST as sets of tuples
                    with open(config_h_path, "r") as file:
                        content = file.read()
                        # Find the line with #define UNUSED_POSITIONS_LIST
                        idx = content.find('#define UNUSED_POSITIONS_LIST')
                        if idx != -1:
                            # Find the first '{' after the define
                            start = content.find('{', idx)
                            if start != -1:
                                brace_count = 0
                                end = start
                                for i, c in enumerate(content[start:], start=start):
                                    if c == '{':
                                        brace_count += 1
                                    elif c == '}':
                                        brace_count -= 1
                                        if brace_count == 0:
                                            end = i
                                            break
                                braces_content = content[start:end+1]
                                # Remove all whitespace for comparison
                                existing_str = re.sub(r"\s+", "", braces_content)
                                generated_str = re.sub(r"\s+", "", f"{{{formatted_pairs}}}")
                                if existing_str == generated_str:
                                    log_file.write(
                                        f"{board_name} ({layout_name}): UNUSED_POSITIONS_LIST already defined in config.h\n"
                                    )
                                else:
                                    log_file.write(
                                        f"{board_name} ({layout_name}): #define UNUSED_POSITIONS_LIST {{ {formatted_pairs} }}\n"
                                    )
                            else:
                                log_file.write(
                                    f"{board_name} ({layout_name}): #define UNUSED_POSITIONS_LIST {{ {formatted_pairs} }}\n"
                                )
                        else:
                            log_file.write(
                                f"{board_name} ({layout_name}): #define UNUSED_POSITIONS_LIST {{ {formatted_pairs} }}\n"
                            )

    # Sort the log file after processing all boards
    sort_log_file(log_file_path)


if __name__ == "__main__":
    main()

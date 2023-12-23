def parse_c_header_to_dict(file_path):
    """
    Parses a C header file and extracts #define directives into a dictionary.

    :param file_path: Path to the C header file.
    :return: Dictionary with keys as macro names and values as macro values.
    """
    defines_dict = {}
    try:
        with open(file_path, "r") as file:
            for line in file:
                # Strip whitespace and check if line starts with '#define'
                if line.strip().startswith("#define"):
                    parts = line.strip().split()
                    if len(parts) >= 3:
                        # The second part is the macro name and the third part is the value
                        defines_dict[parts[1]] = parts[2]
    except FileNotFoundError:
        print(f"File not found: {file_path}")
    except Exception as e:
        print(f"An error occurred: {e}")

    return defines_dict


# Example usage
header_file_path = "/path/to/your/header/file.h"
defines = parse_c_header_to_dict(header_file_path)
print(defines)

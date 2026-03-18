import sys
import fnmatch
import os

def get_impacted_tests(changed_files):
    if not changed_files:
        return ""

    impacted_tests = set()
    full_trigger_patterns = [
        "CMakeLists.txt",
        "FreeRTOSConfig.h",
        ".github/workflows/*",
        "pico_sdk_import.cmake",
    ]
    
    # Mapping patterns to test regexes
    mappings = [
        ("src/hal/*", "SIL_Blinky"),
        ("src/middleware/mw_led.c", "SIL_Blinky"),
        ("test/SIL/test_blinky.c", "SIL_Blinky"),
    ]

    skip_patterns = [
        "README.md",
        ".gitignore",
        ".cppcheck_suppressions",
    ]

    run_all = False
    has_relevant_changes = False

    for f in changed_files:
        # Check for full trigger
        for pattern in full_trigger_patterns:
            if fnmatch.fnmatch(f, pattern):
                run_all = True
                break
        if run_all:
            break

        # Check for skips
        is_skip = False
        for pattern in skip_patterns:
            if fnmatch.fnmatch(f, pattern):
                is_skip = True
                break
        if is_skip:
            continue

        # If it's a source file or test file not explicitly mapped, fallback to run all
        # unless it's explicitly mapped to something.
        matched = False
        for pattern, test_regex in mappings:
            if fnmatch.fnmatch(f, pattern):
                impacted_tests.add(test_regex)
                matched = True
                has_relevant_changes = True
        
        if not matched:
            # Fallback for unmapped source/test files
            if f.startswith("src/") or f.startswith("test/") or f.startswith("libs/") or f.endswith(".c") or f.endswith(".h"):
                run_all = True
                break
            # If we reach here, it might be an unknown file type or in an unknown directory.
            # We don't set has_relevant_changes if it's not a source/test file.

    if run_all:
        return "."
    
    if not impacted_tests and not has_relevant_changes:
        return ""

    return "|".join(sorted(list(impacted_tests)))

if __name__ == "__main__":
    # Expecting space-separated files as multiple arguments or a single string
    files = sys.argv[1:]
    # Handle the case where tj-actions might pass them as a single string if not careful
    if len(files) == 1 and " " in files[0]:
        files = files[0].split()
    
    print(get_impacted_tests(files))

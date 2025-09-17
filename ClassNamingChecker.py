#!/usr/bin/env python3
"""
DXEngine Class Naming Convention Checker

This script checks C++ source files for correct Unreal Engine naming conventions:
- Classes inheriting from UObject should use U prefix
- Regular classes should use F prefix
- Enums should use E prefix
- Templates should use T prefix

Usage: python ClassNamingChecker.py [directory_path]
"""

import os
import re
import sys
from typing import List, Dict, Tuple, Set

class ClassNamingChecker:
    def __init__(self):
        # Patterns for detecting class declarations and inheritance
        self.class_pattern = re.compile(r'^\s*class\s+([A-Z][A-Za-z0-9_]*)\s*(?::\s*public\s+([A-Z][A-Za-z0-9_:]+))?\s*\{?', re.MULTILINE)
        self.enum_pattern = re.compile(r'^\s*enum\s+(?:class\s+)?([A-Z][A-Za-z0-9_]*)', re.MULTILINE)
        self.template_pattern = re.compile(r'^\s*template\s*<[^>]+>\s*class\s+([A-Z][A-Za-z0-9_]*)', re.MULTILINE)

        # Known UObject-derived base classes
        self.uobject_bases = {
            'UObject', 'UEngineSubsystem', 'USceneComponent', 'UActorComponent',
            'UPrimitiveComponent', 'UStaticMeshComponent', 'USkeletalMeshComponent',
            'URenderer', 'UInputManager', 'UGizmoManager', 'UMeshManager'
        }

        # Track inheritance relationships
        self.inheritance_map: Dict[str, str] = {}

    def is_uobject_derived(self, class_name: str) -> bool:
        """Check if a class is derived from UObject (directly or indirectly)"""
        if class_name in self.uobject_bases:
            return True

        # Check inheritance chain
        current = class_name
        visited = set()
        while current in self.inheritance_map and current not in visited:
            visited.add(current)
            current = self.inheritance_map[current]
            if current in self.uobject_bases:
                return True

        return False

    def check_file(self, filepath: str) -> List[Tuple[int, str, str]]:
        """Check a single C++ file for naming convention violations"""
        violations = []

        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading {filepath}: {e}")
            return violations

        lines = content.split('\n')

        # First pass: build inheritance map
        for match in self.class_pattern.finditer(content):
            class_name = match.group(1)
            base_class = match.group(2)
            if base_class:
                # Clean up base class name (remove scope resolution)
                base_class = base_class.split('::')[-1]
                self.inheritance_map[class_name] = base_class

        # Second pass: check naming conventions
        for match in self.class_pattern.finditer(content):
            line_num = content[:match.start()].count('\n') + 1
            class_name = match.group(1)
            base_class = match.group(2)

            # Skip forward declarations and incomplete matches
            if not match.group(0).strip().endswith(('{', ';')):
                continue

            # Determine expected prefix
            if self.is_uobject_derived(class_name) or (base_class and self.is_uobject_derived(base_class)):
                expected_prefix = 'U'
                rule = "UObject-derived classes should use U prefix"
            else:
                expected_prefix = 'F'
                rule = "Regular classes should use F prefix"

            # Check if class name follows convention
            if not class_name.startswith(expected_prefix):
                actual_prefix = class_name[0] if class_name else ''
                violation = f"Class '{class_name}' should start with '{expected_prefix}' (found '{actual_prefix}'). {rule}"
                violations.append((line_num, violation, lines[line_num - 1].strip()))

        # Check enum naming
        for match in self.enum_pattern.finditer(content):
            line_num = content[:match.start()].count('\n') + 1
            enum_name = match.group(1)

            if not enum_name.startswith('E'):
                violation = f"Enum '{enum_name}' should start with 'E' prefix"
                violations.append((line_num, violation, lines[line_num - 1].strip()))

        # Check template naming
        for match in self.template_pattern.finditer(content):
            line_num = content[:match.start()].count('\n') + 1
            template_name = match.group(1)

            if not template_name.startswith('T'):
                violation = f"Template class '{template_name}' should start with 'T' prefix"
                violations.append((line_num, violation, lines[line_num - 1].strip()))

        return violations

    def check_directory(self, directory: str) -> Dict[str, List[Tuple[int, str, str]]]:
        """Check all C++ files in a directory recursively"""
        results = {}

        for root, dirs, files in os.walk(directory):
            # Skip common directories that shouldn't be checked
            dirs[:] = [d for d in dirs if d not in {'.git', '.vs', 'Debug', 'Release', 'x64', 'bin', 'obj', 'Intermediate'}]

            for file in files:
                if file.endswith(('.cpp', '.h', '.hpp', '.cc', '.cxx')):
                    filepath = os.path.join(root, file)
                    violations = self.check_file(filepath)
                    if violations:
                        results[filepath] = violations

        return results

    def print_results(self, results: Dict[str, List[Tuple[int, str, str]]]):
        """Print the results in a readable format"""
        if not results:
            print("✅ No naming convention violations found!")
            return

        total_violations = sum(len(violations) for violations in results.values())
        print(f"❌ Found {total_violations} naming convention violations in {len(results)} files:\n")

        for filepath, violations in results.items():
            print(f"📁 {os.path.relpath(filepath)}")
            for line_num, violation, code_line in violations:
                print(f"   Line {line_num:4d}: {violation}")
                print(f"              Code: {code_line}")
            print()

def main():
    if len(sys.argv) > 1:
        directory = sys.argv[1]
    else:
        directory = '.'

    if not os.path.isdir(directory):
        print(f"Error: '{directory}' is not a valid directory")
        sys.exit(1)

    print(f"🔍 Checking C++ files in '{os.path.abspath(directory)}' for naming convention violations...\n")

    checker = ClassNamingChecker()
    results = checker.check_directory(directory)
    checker.print_results(results)

    # Exit with error code if violations found (useful for CI/CD)
    sys.exit(1 if results else 0)

if __name__ == "__main__":
    main()
#!/usr/bin/env python3
"""
Automated DS Function Call Wrapper
Systematically wraps DS-specific function calls with conditional compilation
for the Pokemon Platinum SDL3 port.
"""

import os
import re
import argparse
from pathlib import Path

# DS-specific function prefixes and patterns
DS_FUNCTION_PATTERNS = [
    r'GX_[A-Za-z0-9_]+',
    r'GXS_[A-Za-z0-9_]+',
    r'NNS_[A-Za-z0-9_]+',
    r'OS_[A-Za-z0-9_]+',
    r'FS_[A-Za-z0-9_]+',
    r'G2_[A-Za-z0-9_]+',
    r'G3D_[A-Za-z0-9_]+',
    r'TP_[A-Za-z0-9_]+',
    r'PAD_[A-Za-z0-9_]+',
    r'SND_[A-Za-z0-9_]+',
]

# Constants that need to be wrapped
DS_CONSTANT_PATTERNS = [
    r'GX_[A-Z0-9_]+',
    r'GXS_[A-Z0-9_]+',
    r'NNS_[A-Z0-9_]+',
    r'OS_[A-Z0-9_]+',
    r'FS_[A-Z0-9_]+',
    r'PAD_[A-Z0-9_]+',
]

# Files to skip (already have PAL implementations or are DS-only)
SKIP_FILES = {
    'src/platform/sdl/',
    'src/platform/ds/',
    'include/platform/',
    'build/',
    'build-sdl/',
}

def should_skip_file(filepath: Path) -> bool:
    """Check if file should be skipped"""
    str_path = str(filepath)
    return any(skip in str_path for skip in SKIP_FILES)

def find_ds_function_calls(content: str) -> list:
    """Find all DS function calls in content"""
    calls = []
    for pattern in DS_FUNCTION_PATTERNS:
        # Match function calls (pattern followed by parentheses)
        matches = re.finditer(rf'\b({pattern})\s*\(', content)
        for match in matches:
            calls.append({
                'type': 'function',
                'name': match.group(1),
                'start': match.start(),
                'end': match.end() - 1,  # Don't include the opening paren
                'line_num': content[:match.start()].count('\n') + 1
            })
    return calls

def find_ds_constants(content: str) -> list:
    """Find all DS constant references in content"""
    constants = []
    for pattern in DS_CONSTANT_PATTERNS:
        # Match constants (not followed by parentheses, not preceded by #define)
        matches = re.finditer(rf'(?<!#define\s)\b({pattern})\b(?!\s*\()', content)
        for match in matches:
            # Skip if it's part of a #define line
            line_start = content.rfind('\n', 0, match.start()) + 1
            line_end = content.find('\n', match.end())
            if line_end == -1:
                line_end = len(content)
            line = content[line_start:line_end]
            if line.strip().startswith('#define'):
                continue
                
            constants.append({
                'type': 'constant',
                'name': match.group(1),
                'start': match.start(),
                'end': match.end(),
                'line_num': content[:match.start()].count('\n') + 1
            })
    return constants

def is_already_wrapped(content: str, call_start: int) -> bool:
    """Check if a call is already wrapped in conditional compilation"""
    # Look backwards from the call to see if we're in a #ifdef block
    lines_before = content[:call_start].split('\n')
    ifdef_count = 0
    
    for line in reversed(lines_before[-20:]):  # Check last 20 lines
        line = line.strip()
        if line.startswith('#ifdef PLATFORM_DS') or line.startswith('#ifndef PLATFORM_SDL'):
            ifdef_count += 1
        elif line.startswith('#else'):
            ifdef_count += 1
        elif line.startswith('#endif'):
            ifdef_count -= 1
            
    return ifdef_count > 0

def wrap_ds_call(content: str, call: dict) -> str:
    """Wrap a DS call with conditional compilation"""
    lines = content.split('\n')
    line_idx = call['line_num'] - 1
    line = lines[line_idx]
    
    # Get indentation
    indent = len(line) - len(line.lstrip())
    indent_str = ' ' * indent
    
    # Check if line is already wrapped
    if is_already_wrapped(content, call['start']):
        return content
    
    # Create the wrapped version
    if call['name'].startswith(('GX_', 'GXS_', 'G2_', 'G3D_')):
        # Graphics calls - replace with PAL stub
        replacement = f"{indent_str}#ifdef PLATFORM_DS\n{line}\n{indent_str}#else\n{indent_str}// TODO: Port {call['name']} to PAL\n{indent_str}#endif"
    elif call['name'].startswith('OS_'):
        # OS calls - replace with PAL stub
        replacement = f"{indent_str}#ifdef PLATFORM_DS\n{line}\n{indent_str}#else\n{indent_str}// TODO: Port {call['name']} to PAL\n{indent_str}#endif"
    elif call['name'].startswith(('NNS_', 'FS_')):
        # Nintendo system calls
        replacement = f"{indent_str}#ifdef PLATFORM_DS\n{line}\n{indent_str}#else\n{indent_str}// TODO: Port {call['name']} to PAL\n{indent_str}#endif"
    elif call['name'].startswith(('PAD_', 'TP_')):
        # Input calls - we have PAL for these
        if call['name'] == 'PAD_Read':
            replacement = f"{indent_str}#ifdef PLATFORM_DS\n{line}\n{indent_str}#else\n{indent_str}{line.replace('PAD_Read()', 'PAL_Input_GetHeld()')}\n{indent_str}#endif"
        else:
            replacement = f"{indent_str}#ifdef PLATFORM_DS\n{line}\n{indent_str}#else\n{indent_str}// TODO: Port {call['name']} to PAL\n{indent_str}#endif"
    else:
        # Generic DS call
        replacement = f"{indent_str}#ifdef PLATFORM_DS\n{line}\n{indent_str}#else\n{indent_str}// TODO: Port {call['name']} to PAL\n{indent_str}#endif"
    
    lines[line_idx] = replacement
    return '\n'.join(lines)

def process_file(filepath: Path, dry_run: bool = False) -> dict:
    """Process a single file and wrap DS calls"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        return {'error': str(e)}
    
    original_content = content
    
    # Find DS calls
    function_calls = find_ds_function_calls(content)
    constants = find_ds_constants(content)
    
    if not function_calls and not constants:
        return {'status': 'no_changes', 'functions': 0, 'constants': 0}
    
    # Sort by position (reverse order to avoid offset issues)
    all_calls = function_calls + constants
    all_calls.sort(key=lambda x: x['start'], reverse=True)
    
    changes_made = 0
    
    # Process each call
    for call in all_calls:
        if not is_already_wrapped(content, call['start']):
            content = wrap_ds_call(content, call)
            changes_made += 1
    
    if changes_made == 0:
        return {'status': 'no_changes', 'functions': len(function_calls), 'constants': len(constants)}
    
    # Write changes if not dry run
    if not dry_run and content != original_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
    
    return {
        'status': 'modified',
        'functions': len(function_calls),
        'constants': len(constants),
        'changes_made': changes_made,
        'dry_run': dry_run
    }

def main():
    parser = argparse.ArgumentParser(description='Wrap DS function calls with conditional compilation')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be changed without modifying files')
    parser.add_argument('--src-dir', default='src', help='Source directory to process (default: src)')
    parser.add_argument('--include-dir', default='include', help='Include directory to process (default: include)')
    parser.add_argument('--file', help='Process a single file instead of directories')
    
    args = parser.parse_args()
    
    if args.file:
        # Process single file
        filepath = Path(args.file)
        if not filepath.exists():
            print(f"❌ File not found: {filepath}")
            return
        
        print(f"Processing {filepath}...")
        result = process_file(filepath, args.dry_run)
        
        if 'error' in result:
            print(f"❌ Error: {result['error']}")
        elif result['status'] == 'no_changes':
            print(f"✅ No DS calls found")
        else:
            status = "Would modify" if args.dry_run else "Modified"
            print(f"✅ {status} {result['changes_made']} calls ({result['functions']} functions, {result['constants']} constants)")
        return
    
    # Process directories
    src_dir = Path(args.src_dir)
    include_dir = Path(args.include_dir)
    
    if not src_dir.exists():
        print(f"❌ Source directory not found: {src_dir}")
        return
    
    print("🔍 Scanning for DS function calls...")
    print(f"   Source: {src_dir}")
    if include_dir.exists():
        print(f"   Headers: {include_dir}")
    
    if args.dry_run:
        print("🔄 DRY RUN - No files will be modified")
    
    print("")
    
    total_files = 0
    modified_files = 0
    total_functions = 0
    total_constants = 0
    total_changes = 0
    
    # Process all C files
    for directory in [src_dir, include_dir]:
        if not directory.exists():
            continue
            
        for filepath in directory.rglob('*.c'):
            if should_skip_file(filepath):
                continue
                
            total_files += 1
            result = process_file(filepath, args.dry_run)
            
            if 'error' in result:
                print(f"❌ {filepath}: {result['error']}")
            elif result['status'] == 'modified':
                modified_files += 1
                total_functions += result['functions']
                total_constants += result['constants']
                total_changes += result['changes_made']
                
                status = "Would modify" if args.dry_run else "Modified"
                print(f"✅ {filepath}: {status} {result['changes_made']} calls")
            elif result['functions'] > 0 or result['constants'] > 0:
                print(f"ℹ️  {filepath}: {result['functions']} functions, {result['constants']} constants (already wrapped)")
        
        # Process header files
        for filepath in directory.rglob('*.h'):
            if should_skip_file(filepath):
                continue
                
            total_files += 1
            result = process_file(filepath, args.dry_run)
            
            if 'error' in result:
                print(f"❌ {filepath}: {result['error']}")
            elif result['status'] == 'modified':
                modified_files += 1
                total_functions += result['functions']
                total_constants += result['constants']
                total_changes += result['changes_made']
                
                status = "Would modify" if args.dry_run else "Modified"
                print(f"✅ {filepath}: {status} {result['changes_made']} calls")
    
    print("")
    print("📊 Summary:")
    print(f"   Files processed: {total_files}")
    print(f"   Files modified: {modified_files}")
    print(f"   Function calls found: {total_functions}")
    print(f"   Constants found: {total_constants}")
    print(f"   Total changes: {total_changes}")
    
    if args.dry_run and total_changes > 0:
        print("")
        print("🚀 Run without --dry-run to apply these changes")
    elif total_changes > 0:
        print("")
        print("✅ All changes applied successfully!")
        print("   Run ./build_sdl.sh to test compilation")

if __name__ == '__main__':
    main()
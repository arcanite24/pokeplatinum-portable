#!/usr/bin/env python3
"""
Automated Header Dependency Fixer
Adds missing type definitions to header files for the Pokemon Platinum SDL3 port.
"""

import os
import re
from pathlib import Path

def fix_header_includes(filepath: Path) -> bool:
    """Fix missing includes in header files"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"❌ Error reading {filepath}: {e}")
        return False
    
    original_content = content
    lines = content.split('\n')
    
    # Check if we need to add includes
    needs_types = False
    needs_global = False
    
    # Check for type usage
    if re.search(r'\b(BOOL|u8|u16|u32|s8|s16|s32)\b', content):
        needs_types = True
    
    # Check for constants usage
    if re.search(r'\b(JAPANESE|ENGLISH|ITALIAN|SPANISH|FRENCH|GERMAN|KOREAN)\b', content):
        needs_global = True
    
    if not needs_types and not needs_global:
        return False
    
    # Find insertion point (after header guard, before first declaration)
    insert_idx = 0
    for i, line in enumerate(lines):
        if line.startswith('#ifndef') or line.startswith('#define'):
            continue
        if line.strip() == '':
            continue
        # Found first non-guard, non-empty line
        insert_idx = i
        break
    
    # Add includes
    includes_to_add = []
    
    if needs_types:
        includes_to_add.append('#include "platform/platform_types.h"')
    
    if needs_global:
        includes_to_add.append('#include "constants/global.h"')
    
    # Insert includes
    for include in reversed(includes_to_add):
        lines.insert(insert_idx, include)
        lines.insert(insert_idx + 1, '')  # Add empty line after include
    
    new_content = '\n'.join(lines)
    
    if new_content != original_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        return True
    
    return False

def main():
    """Fix header files in include directory"""
    include_dir = Path('include')
    
    if not include_dir.exists():
        print("❌ Include directory not found")
        return
    
    print("🔍 Scanning header files for missing type definitions...")
    
    fixed_files = 0
    total_files = 0
    
    for filepath in include_dir.rglob('*.h'):
        # Skip platform files
        if 'platform/' in str(filepath):
            continue
            
        total_files += 1
        
        if fix_header_includes(filepath):
            fixed_files += 1
            print(f"✅ Fixed {filepath}")
    
    print(f"\n📊 Summary:")
    print(f"   Header files processed: {total_files}")
    print(f"   Header files fixed: {fixed_files}")
    
    if fixed_files > 0:
        print("\n✅ Header includes added successfully!")

if __name__ == '__main__':
    main()
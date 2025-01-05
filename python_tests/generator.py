import random
import string
import sys
import os

def generate_random_section(size=1000, section_num=1):
    """Generate a section of random ASCII text with markers."""
    section = []
    section.append(f"[SECTION-{section_num:04d}-START]\n")
    
    # Generate different types of content
    patterns = [
        # Random printable ASCII
        ''.join(random.choices(string.printable[:-6], k=size//4)),
        # Random letters and numbers
        ''.join(random.choices(string.ascii_letters + string.digits, k=size//4)),
        # Random punctuation mixed with letters
        ''.join(random.choices(string.ascii_letters + string.punctuation, k=size//4)),
        # Random everything
        ''.join(random.choices(string.printable[:-6], k=size//4))
    ]
    
    section.extend(patterns)
    section.append(f"\n[SECTION-{section_num:04d}-END]\n")
    return ''.join(section)

def generate_test_file(total_size_kb=50):
    """Generate a complete test file with headers and sections."""
    bytes_per_section = 1024  # 1KB sections
    num_sections = (total_size_kb * 1024) // bytes_per_section
    
    # Header
    content = [
        "=== TEST FILE HEADER ===\n",
        f"Target Size: {total_size_kb}KB\n",
        f"Number of Sections: {num_sections}\n",
        "Format: Random ASCII\n",
        "======================\n\n"
    ]
    
    # Generate sections
    for i in range(num_sections):
        content.append(generate_random_section(bytes_per_section, i+1))
        if i % 10 == 0:  # Add markers every 10 sections
            content.append(f"\n=== MILESTONE {i//10 + 1} ===\n\n")
    
    # Footer
    content.extend([
        "\n=== TEST FILE FOOTER ===\n",
        "Transmission Complete\n",
        "======================"
    ])
    
    return ''.join(content)

def write_to_file(content, filename='input.txt'):
    """Write content to file, creating or replacing if exists."""
    try:
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(content)
        return True
    except Exception as e:
        print(f"Error writing to file: {e}", file=sys.stderr)
        return False

if __name__ == "__main__":
    # Default to 50KB if no size specified
    size_kb = 50 if len(sys.argv) < 2 else int(sys.argv[1])
    print(f"Generating {size_kb}KB test file...", file=sys.stderr)
    
    # Generate content
    test_data = generate_test_file(size_kb)
    
    # Write to file
    if write_to_file(test_data):
        file_size = os.path.getsize('input.txt')
        print(f"Successfully wrote {file_size/1024:.2f}KB to input.txt", file=sys.stderr)
    else:
        print("Failed to write to input.txt", file=sys.stderr)
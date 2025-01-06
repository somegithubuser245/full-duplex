import time
import os
import sys
from datetime import datetime

def generate_test_file(size_kb, filename="benchmark_input.txt"):
    """Generate a test file of specific size in KB"""
    # Generate repeatable pattern for easy verification
    chunk = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    
    bytes_to_write = size_kb * 1024
    repeats = (bytes_to_write + len(chunk) - 1) // len(chunk)
    
    with open(filename, 'w') as f:
        f.write((chunk * repeats)[:bytes_to_write])
    
    return filename

def run_benchmark(size_kb):
    """Run a complete benchmark"""
    input_file = generate_test_file(size_kb)
    output_file = "benchmark_output.txt"
    
    print(f"Starting benchmark with {size_kb}KB file...")
    print(f"Generated input file size: {os.path.getsize(input_file)} bytes")
    
    # Record start time
    start_time = time.time()
    
    # Run the protocol
    os.system(f"cat {input_file} | sudo ../src/comm_program -f > {output_file}")
    
    # Record end time
    end_time = time.time()
    
    # Calculate statistics
    duration = end_time - start_time
    input_size = os.path.getsize(input_file)
    
    if os.path.exists(output_file):
        output_size = os.path.getsize(output_file)
        
        # Calculate transfer rates
        bits_per_second = (input_size * 8) / duration
        bytes_per_second = input_size / duration
        kilobytes_per_second = bytes_per_second / 1024
        
        print("\nBenchmark Results:")
        print(f"Duration: {duration:.2f} seconds")
        print(f"Input file size: {input_size} bytes")
        print(f"Output file size: {output_size} bytes")
        print(f"Transfer rate: {bits_per_second:.2f} bits/second")
        print(f"             = {bytes_per_second:.2f} bytes/second")
        print(f"             = {kilobytes_per_second:.2f} KB/second")
        
        # Verify data integrity
        if os.system(f"diff {input_file} {output_file} > /dev/null") == 0:
            print("\nData integrity: PASSED ✓")
        else:
            print("\nData integrity: FAILED ✗")
    else:
        print("Error: Output file was not created")

if __name__ == "__main__":
    try:
        size_kb = int(sys.argv[1]) if len(sys.argv) > 1 else 100
        run_benchmark(size_kb)
    except KeyboardInterrupt:
        print("\nBenchmark interrupted by user")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        # Cleanup
        if os.path.exists("benchmark_input.txt"):
            os.remove("benchmark_input.txt")
        if os.path.exists("benchmark_output.txt"):
            os.remove("benchmark_output.txt")

#testing out git
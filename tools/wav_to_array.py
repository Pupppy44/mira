# mira/tools/wav_to_array.py
# Converts WAV files to raw PCM C arrays for Mira (Sound Blaster 16 compatible)

import subprocess
import glob
import os
import threading

# WAV to PCM conversion using ffmpeg
def wav_to_pcm_array(wav_file_path):
    # Raw Command: ffmpeg -i input.wav -f u8 -ar 8000 -ac 1 output.raw
    pcm_file_path = wav_file_path.replace(".wav", ".raw")
    ffmpeg_command = [
        "D:\\X\\ffmpeg.exe",
        "-i", wav_file_path,
        "-f", "u8", # u8 = Unsigned 8-bit PCM
        "-ar", "8000", # 8000 Hz
        "-ac", "1", # Mono
        pcm_file_path
    ]
    print(f"Converting {wav_file_path} to PCM...")
    subprocess.run(ffmpeg_command, check=True, capture_output=True) # capture_output=True to suppress ffmpeg output

    with open(pcm_file_path, 'rb') as pcm_file:
        pcm_data = pcm_file.read()

    #os.remove(pcm_file_path) # Clean up the intermediate PCM file
    return list(pcm_data) # Convert bytes to list of integers

def save_pcm_array_to_file(pcm_array, file_path):
    """
    Saves a PCM byte array to a C header file as a uint8_t array.
    """
    array_name = os.path.splitext(os.path.basename(file_path))[0]
    with open(file_path, 'w') as f:
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define {array_name.upper()}_SIZE {len(pcm_array)}\n\n")
        f.write(f"uint8_t {array_name}[] = {{")
        for sample in pcm_array:
            f.write(f"{sample}, ")
        f.write("};")

    print(f"Saved {file_path}")

if __name__ == "__main__":
    wav_files = glob.glob('../assets/*.wav') # Changed to look for wav files in current dir
    print("# of wav files found:", len(wav_files))
    print("Now converting to PCM C arrays...")

    def process_wav_file(wav_file):
        pcm_array = wav_to_pcm_array(wav_file)
        header_file = os.path.splitext(wav_file)[0] + '.h'
        save_pcm_array_to_file(pcm_array, header_file)

    # Use threading to speed up the process for multiple files
    threads = []
    for wav_file in wav_files:
        thread = threading.Thread(target=process_wav_file, args=(wav_file,))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()
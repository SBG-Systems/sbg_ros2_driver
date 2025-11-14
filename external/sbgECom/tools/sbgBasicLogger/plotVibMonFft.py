#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2007-2025 SBG Systems. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def print_help():
    """
    Display usage instructions.
    """

    print("Usage: python plotVibMonFft.py <input_file>")
    print("  <input_file>: Path to the vibration FFT data file (tab-separated format)")
    print("Example:")
    print("  python plotVibMonFft.py vibFftAcf_z_flatTop.txt")

def main():
    # Constants
    G0            = 9.80665               # Standard gravity in m/s^2
    MAG_MIN_SCALE = 1.0                   # Minimul amplitude scale when the signal is too low

    # Handle help and input arguments
    if len(sys.argv) != 2 or sys.argv[1] in ("--help", "-h"):
        print_help()
        return

    file_path = sys.argv[1]

    # Read file and handle any file access errors
    try:
        with open(file_path, 'r') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error opening file: {e}")
        return

    # Parse frequency bin labels (2nd line in file, after the header)
    try:
        freqs = list(map(float, lines[1].strip().split('\t')[1:]))
    except Exception:
        print("Error: Unable to parse frequency axis.")
        return
    
    # Load the FFT data, skipping metadata lines
    data = pd.read_csv(file_path, sep='\t', skiprows=3)

     # Extract timestamps and FFT magnitudes
    timestamps = data.iloc[:, 0].values     # Raw timestamps in µs
    magnitudes = data.iloc[:, 1:].values    # Magnitude acceleration in m/s^2

    # Convert timestamps from microseconds to seconds (relative to start)
    timestamps = (timestamps - timestamps[0]) * 1e-6

    # Convert FFT magnitudes from m/s^2 to g
    magnitudes_g = magnitudes / G0

    # Determine color scale limits for visualization
    vmin = 0
    vmax = np.ceil(max(np.max(magnitudes_g), MAG_MIN_SCALE))

    # Round frequency axis limits to nearest 10 Hz for better readability
    fmin = np.floor(np.min(freqs)/10.0)*10
    fmax = np.ceil(np.max(freqs)/10.0)*10
    
    # Create the spectrogram plot
    plt.figure(figsize=(12, 6))
    plt.pcolormesh(timestamps, freqs, magnitudes_g.T, shading='auto', vmin=vmin, vmax=vmax, cmap='viridis')
    plt.colorbar(label='Magnitude (g)')
    plt.ylabel('Frequency (Hz)')
    plt.xlabel('Time (s)')
    plt.title('Vibration FFT Spectrogram (in g)')
    plt.ylim(fmin, fmax)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()

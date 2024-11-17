#/bin/bash

# IFJ24 - Priklady pro studenty pro dokresleni zadani (spusteni konfigurovano pro server Merlin)

COMPILER=../../$1  # zde muze byt vas prekladac IFJ24 (napr. ./ifj24)
INTERPRETER=../../$2  # zde muze byt interpret IFJcode24 (./ic24int)
path_in=./in

# Define color codes
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
RESET='\033[0m'

# Check for required arguments
if [ $# -lt 1 ]; then
    echo "Usage: $0 <test_binaries...>"
    exit 1
fi


for code in "$path_in"/*.ifj; do

    name_of_input="${code%.ifj}"
	# Find all input files matching the pattern
	for input_file in "$name_of_input".in*; do
		output_file="${input_file%.in}.out"
		ref_file="${input_file%.in}.ref"

		./$(COMPILER) < "$input_file" > "$output_file"

		# Check if a reference file exists
		if [ -f "$ref_file" ]; then
			if diff -q "$output_file" "$ref_file" > /dev/null; then
				echo -e "${GREEN}Test passed: $input_file${RESET}"
			else
				echo -e "${RED}Output differs from reference file!${RESET}"
				echo "$test $input_file $output_file"
				echo -e "${YELLOW}Differences:${RESET}"
				diff "$output_file" "$ref_file"
			fi
		else
			echo -e "${YELLOW}No .ref file for test: $input_file${RESET}"
		fi
	done
done


exit 0

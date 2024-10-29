import os, sys
from subprocess import Popen, PIPE

class CustomError(Exception):
    def __init__(self, message):
        self.message = f"\033[91;1;4m{message}:\033[0m"
        super().__init__(self.message)

def delete_multiple_spaces(string: str) -> str:
    return ' '.join(string.split())

def parse_line(line: str) -> dict:
    line = delete_multiple_spaces(line)

    data = dict()

    for items in line.split(' and '):
        item = items.split(' ')

        if len(item) == 1:
            key = item[0]
            value = "True"
        elif len(item) == 2:
            key = item[1]
            value = int(item[0])
        else:
            raise CustomError(f"Invalid format for line")
        
        if key in data:
            raise CustomError(f'Key {key} is repeated in output')
        else:
            data[key] = value

    return data

def line_diff_error_message(output_file: str, line_number: int, line: str, expected_line: str) -> str:
    return f"\033[91mError at line {line_number} in file '{output_file}':\n\toutput is \033[90m'{line}'\033[91m but expected: \033[90m'{expected_line}'\033[0m"


def line_diff(line: str, expected_line: str, context: dict) -> bool:
    try:
        data = parse_line(line)
        expected_data = parse_line(expected_line)
        for key in expected_data:
            if key not in data:
                raise CustomError(f'Key {key} not in output')
            if data[key] != expected_data[key]:
                raise CustomError(f'Value {data[key]} for key {key} is not equal to {expected_data[key]}')

        for key in data:
            if key not in expected_data:
                raise CustomError(f'Key {key} not in expected output')
            if data[key] != expected_data[key]:
                raise CustomError(f'Value {data[key]} for key {key} is not equal to {expected_data[key]}'    )
        return True
    
    except Exception as e:
        print(e.message, file=sys.stderr)
        print(line_diff_error_message(context['output_file'], context['line_number'], line, expected_line), file=sys.stderr)
        return False

def check_output(output_file_name: str, expected_output_file_name: str) -> float:
    
    output_file = open(output_file_name, 'r')
    expected_output_file = open(expected_output_file_name, 'r')

    output = output_file.read().strip().split('\n')
    expected_output = expected_output_file.read().strip().split('\n')

    output = [line for line in output if line.strip()]
    expected_output = [line for line in expected_output if line.strip()]

    sum = 0
    
    for i in range(min(len(output), len(expected_output))):
        if line_diff(output[i], expected_output[i], {'output_file': output_file_name, 'line_number': i}):
            sum += 1
      
    return sum / len(expected_output)

def run(executable: str, input_file_name: str, output_file_name: str, expected_output_file_name: str) -> float:
    
    input_file = open(input_file_name, 'r')
    output_file = open(output_file_name, 'w')

    process = Popen([f'./{executable}'], stdout=PIPE, stdin=PIPE, universal_newlines=True)

    for line in input_file.readlines():

        prompt = process.stdout.read(3)
        
        if prompt != '>> ':
            continue

        process.stdin.write(line.replace('\n', '') + '\n')
        process.stdin.flush()

        if line == 'exit':
            break

        output_file.write(process.stdout.readline())

    input_file.close()
    output_file.close()
    process.stdin.close()
    process.stdout.close()

    return check_output(output_file_name, expected_output_file_name)

if __name__ == '__main__':

    if len(sys.argv) != 5:
        print('Usage: python checker.py <executable> <input_file> <output_file> <expected_output_file>')
        sys.exit(1)

    print(run(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]))
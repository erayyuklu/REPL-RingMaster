import os, sys
import checker as checker
import multiprocessing

def get_grade(executable: str, test_cases_folder: str) -> float:
    
    grade = 0
    number_of_test_cases = 0

    if not os.path.exists('my-outputs'):
        os.makedirs('my-outputs')
    
    os.system('make')

    for input_file in os.listdir(f'{test_cases_folder}'):
        if not input_file.startswith('input'):
            continue
        output_file = input_file.replace('input', 'output')
        number_of_test_cases += 1
 
        test_grade = 0
        with multiprocessing.Pool(1) as pool:
            result = pool.apply_async(checker.run, (executable, f'{test_cases_folder}/{input_file}', f'my-outputs/{output_file}', f'{test_cases_folder}/{output_file}'))
            try:
                test_grade = result.get(timeout=60)
            except Exception as e:
                test_grade = 0

        grade += test_grade

        print(f"\033[93mtest-case '{input_file}': {test_grade*100:.2f} points\033[0m", file=sys.stderr)
    
    if grade ==0:
        return 0  
    else:
        return grade * 100 / number_of_test_cases

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: python grader.py <executable> <test_cases_folder>')
        sys.exit(1)

    grade = get_grade(sys.argv[1], sys.argv[2])
    
    print(f'\033[92mGrade = {grade:.2f}\033[0m', file=sys.stderr)
    print(f'{grade:.2f}')

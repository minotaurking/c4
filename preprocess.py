"""
Preprocessor for C code

Replace ASSERT with assert, if assert failed, output file name
line number and assert condition
"""
import sys
import re

def replace_assert(text, file, ln):
    """
    Replace ASSERT with assert
    For example there is an ASSERT statement in test.c, the line number is 20
    Original: ASSERT(i == 3)
    New: assert("test.c", 20, "i == 3", i == 3)
    """
    def replacement(match):
        condition = match.group(1)  # 获取括号里的内容
        # 创建新的格式：assert(file, line, "condition", condition)
        return f'assert("{file}", {ln}, "{condition}", {condition})'

    # 使用 re.sub 进行替换
    pattern = r'ASSERT\((.*?)\)'  # 匹配 ASSERT() 模式
    result = re.sub(pattern, replacement, text)
    return result

def main():
    """
    Main function
    """
    if len(sys.argv) < 2:
        print("Usage: python preprocess.py <input_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = input_file + "c"

    # Get only file name without path from input_file
    src_file = input_file.split('/')[-1]
    ln = 0

    # Open input_file for read, and open output_file for write
    with open(input_file, 'r', encoding='utf-8') as f, \
         open(output_file, 'w', encoding='utf-8') as out:
        for line in f:
            ln = ln + 1
            processed_line = replace_assert(line, src_file, ln)
            out.write(processed_line)

if __name__ == "__main__":
    main()

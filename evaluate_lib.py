import subprocess
from contextlib import suppress


fibonacci_input = range(7, 36, 7)
virtual_processors = range(3, 16, 3)

output_file = "Evaluation.csv"

header = "," + "".join([f"{vp}," for vp in virtual_processors]) + '\n'

with open(output_file, 'w') as output:
    output.write(header)


def get_time(fib_output):
    fib, _, time_output, *_ = fib_output.split('\n')
    _, real_time = time_output.split('\t')

    print(fib)

    return_time = real_time.replace('m', ':') \
                           .replace(',', '.')

    return return_time[:-1]


for fib_in in fibonacci_input:
    with open(output_file, 'a') as output:
        output.write(f"{fib_in},")

    for pv in virtual_processors:
        cmd = f"time ./exemplo {pv} {fib_in}"

        output = ""

        # Executa comando
        while not output:
            # As vezes o comando retorna um erro
            with suppress(subprocess.CalledProcessError):
                output = subprocess.check_output(['bash', '-c', cmd],
                                                 stderr=subprocess.STDOUT)

        output_str = output.decode("utf-8")

        # Pega saída
        time = get_time(output_str)

        # Salva em arquivo de saída
        with open(output_file, 'a') as output:
            output.write(f"{time},")

    with open(output_file, 'a') as output:
        output.write("\n")

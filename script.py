total = 0

with open("Engine") as file:
    for line in file:
        try:
            total += int(line.strip())
        except ValueError:
            continue  # skip lines that aren't numbers

print(total)

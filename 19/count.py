import re
f = open("regex")
template = f.read()
rules = [re.compile(template.format("{" + str(i) + "}", "{" + str(i) + "}")) for i in range(1, 6)]
f.close()
f = open("input.txt")
while(next(f) != "\n"):
    pass
i = 0
for line in f:
    if any([rule.match(line.strip()) for rule in rules]):
        i += 1
print(i)

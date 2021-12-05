import os

def pairs(file):
    for i,current in enumerate(file):
        if i % 2 == 0:
            first = current
        else:
            yield (first, current)


input = open('3-line-games.txt', 'r')

for i,pair in enumerate(pairs(input)):
    with open('{0:02d}'.format(i) + '.txt', 'w') as output:
        output.write(pair[0])
        output.write(pair[1])

# with command:
# split -d -l 2 0_games.txt

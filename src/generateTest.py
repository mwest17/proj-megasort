import random
with open("input.txt", "w") as file:
     for i in range(0, 1000):
          file.write(str(random.randint(1, 100)) + '\n')
file.close()
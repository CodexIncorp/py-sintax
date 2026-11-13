lista = [1, 2, 3, 4, 5]

map(lambda x: x * 2, lista)
filter(cuadrado, lista)

reduce(lambda x, y: x + y, lista)

def cuadrado(x):
    return x ** 2

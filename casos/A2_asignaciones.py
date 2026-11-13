# listas, tuplas y llamadas como valor de una asignacion
datos = [10, 20, 30,]
par = (1, 9,)
doble = map(lambda n: (n + 5) // 2, datos)
pares = filter(lambda x: x % 2 == 0, range(10))
total = reduce(lambda a, b: a + b, datos, 100)

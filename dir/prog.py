x = [1, 2, 3, 4]
y = map(lambda a: a + 2, x)
z = filter(lambda b: b < 3, x)
r = reduce(lambda c, d: c + d, x)
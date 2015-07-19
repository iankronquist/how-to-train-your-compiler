def is_collatz(candidate):
    if candidate == 1:
        return True
    elif candidate % 2 == 0:
        candidate = candidate / 2
    else:
        candidate = 3 * candidate + 1
    return is_collatz(candidate)

print "Is 27 a collatz number?"
if is_collatz(27):
    print "Yes!"
else:
    print "No :("

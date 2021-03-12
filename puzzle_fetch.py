import json
import requests

## URL + parameters
URL = "http://www.cs.utep.edu/cheon/ws/sudoku/new/"
SIZE = 9
LEVEL = 3

for x in range(50):
    ## Array of numbers, represents our sudoku board
    board = ['_'] * (SIZE ** 2)

    ## Get the puzzle from the site, fill in the given values. This creates our initial puzzle setup
    r = requests.get(url=f"{URL}?size={SIZE}&level={LEVEL}")
    with open(f"puzzles_unsolved/puzzle_{x}", "w+") as file:
        for coordinate in r.json()['squares']:
            x = coordinate.get('x')
            y = coordinate.get('y')
            val = coordinate.get('value', '_')

            ## Math to index into 1D array as a 2D array
            board[y*9 + x] = str(val)

        ## Write out our unsolved boards
        for row in range(SIZE):
            print(' '.join(board[row*SIZE:row*SIZE + SIZE]), file=file)

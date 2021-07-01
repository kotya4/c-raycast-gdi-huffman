# adds specific prefix_from to image names
from os import walk, rename

path = 'images/'

_, _, filenames = next ( walk ( path + '.' ) )

prefix_from = 'A'
prefix_to = 'S'

bmps = [ f for f in filenames if f[ -4: ] == '.bmp' and ( len ( prefix_from ) != 1 or f[ 0 ] == prefix_from ) ]

for bmp in bmps :
    rename ( path + bmp, path + prefix_to + ( bmp[ 1: ] if len ( prefix_from ) == 1 else bmp ) )

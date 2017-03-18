import os
import sys
joinp = os.path.join
sys.path.insert(0, 'whitgl')
sys.path.insert(0, joinp('whitgl', 'input'))
import build

sys.path.insert(0, 'input')
import ninja_syntax

build.do_game('Game', '', ['png','ogg'])

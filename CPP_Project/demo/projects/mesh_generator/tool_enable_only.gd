"""
TOOL WRAPPER ONLY

i cannot figure out how to make tool mode work from c++, i am not sure it's possible

so this just enables tool mode, which allows gdscript to interact while in tool mode
(note, much c++ still runs regardless even if tool mode is off)


given how much boilerplate c++ has, a 2 lines script may be a good compromise!

"""
@tool
extends Node

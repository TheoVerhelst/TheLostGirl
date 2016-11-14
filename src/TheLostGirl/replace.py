#!/usr/bin/python3
from sys import argv
from re import sub

# Here is the pattern that will be matched
pattern = "get(Parameters|Window|PostEffectsTexture|TextureManager|FontManager|SoundManager|ScriptManager|LangManager|Gui|EventManager|EntityManager|SystemManager|World|Player|StateStack)\(\)"

replace = r"Context::get\1()"

for fname in argv[1:]:
	print("Processing " + fname)
	s = ""
	try:
		with open(fname) as f:
			s = f.read()

		s = sub(pattern, replace, s)

		with open(fname, "w") as f:
			f.write(s)

	except IOError as e:
		print("Error: " + str(e))


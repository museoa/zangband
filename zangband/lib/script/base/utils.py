class proxy:
	def __init__(self, name, module, class_name):
		self.name = name
		self.module = module
		self.class_name = class_name
	def __call__(self):
		mod = __import__(self.module, globals(), locals(), self.class_name)
		_class = getattr(mod, self.class_name)
		return apply(_class, ())


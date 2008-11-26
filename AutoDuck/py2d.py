import sys
import types

class DocInfo:
    def __init__(self, name, ob):
        self.name = name
        self.ob = ob
        self.short_desc = ""
        self.desc = ""

def BuildArgInfos(ob):
    ret = []
    vars = list(ob.func_code.co_varnames[:ob.func_code.co_argcount])
    vars.reverse() # for easier default checking.
    defs = list(ob.func_defaults or [])
    for i, n in enumerate(vars):
        info = DocInfo(n, ob)
        info.short_desc = info.desc = n
        info.default = ""
        if len(defs):
            info.default = defs.pop()
        ret.append(info)
    ret.reverse()
    return ret

def BuildInfo(name, ob):
    ret = DocInfo(name, ob)
    docstring = ob.__doc__ or ""
    ret.desc = ret.short_desc = docstring.strip()
    if ret.desc:
        ret.short_desc = ret.desc.splitlines()[0]
    return ret

def format_desc(desc):
    if not desc:
        return ""
    lines = desc.splitlines()
    chunks = [lines[0]]
    for line in lines[1:]:
        line = line.strip()
        if not line:
            line = "<nl>"
        chunks.append( "// " + line )
    return "\n".join(chunks)

def build_module(fp, mod_name):
    __import__(mod_name)
    mod = sys.modules[mod_name]
    functions = []
    classes = []
    constants = []
    for name, ob in mod.__dict__.items():
        if name.startswith("_"):
            continue
        if hasattr(ob, "__module__") and ob.__module__ != mod_name:
            continue
        if type(ob)==types.ClassType:
            classes.append(BuildInfo(name, ob))
        elif type(ob)==types.FunctionType:
            functions.append(BuildInfo(name, ob))
        elif name.upper()==name and type(ob) in (int, str):
            constants.append( (name, ob) )
    info = BuildInfo(mod_name, mod)
    print("// @module %s|%s" % (mod_name, format_desc(info.desc)), file=fp)
    for ob in functions:
        print("// @pymeth %s|%s" % (ob.name, ob.short_desc), file=fp)
    for ob in functions:
        print("// @pymethod |%s|%s|%s" % (mod_name, ob.name, format_desc(ob.desc)), file=fp)
        for ai in BuildArgInfos(ob.ob):
            print("// @pyparm |%s|%s|%s" % (ai.name, ai.default, ai.short_desc), file=fp)

    for ob in classes:
        ob_name = mod_name + "." + ob.name
        print("// @object %s|%s" % (ob_name, format_desc(ob.desc)), file=fp)
        func_infos = []
        for n, o in ob.ob.__dict__.items():
            if type(o)==types.FunctionType:
                info = BuildInfo(n, o)
                func_infos.append(info)
        for fi in func_infos:
            print("// @pymeth %s|%s" % (fi.name, fi.short_desc), file=fp)
        for fi in func_infos:
            print("// @pymethod |%s|%s|%s" % (ob_name, fi.name, format_desc(fi.desc)), file=fp)
            for ai in BuildArgInfos(fi.ob):
                print("// @pyparm |%s|%s|%s" % (ai.name, ai.default, ai.short_desc), file=fp)
                
    for (name, val) in constants:
        desc = "%s = %r" % (name, val)
        if type(val) in (int, long):
            desc += " (0x%x)" % (val,)
        print("// @const %s|%s|%s" % (mod_name, name, desc), file=fp)

def main(fp, args):
    print("// @doc", file=fp)
    for arg in args:
        build_module(sys.stdout, arg)

if __name__=='__main__':
    main(sys.stdout, sys.argv[1:])

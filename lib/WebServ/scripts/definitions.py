import abc
import json
import os
import re

try:
    Import("env", "pio_lib_builder")
except NameError:
    pass


def _library_dir():
    try:
        return os.path.abspath(pio_lib_builder.path)
    except NameError:
        return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


ASSET_DIR = os.path.join(_library_dir(), "assets")
OVERRIDE_DIR = "asset"
OUTPUT_FILE = os.path.join(_library_dir(), "definitions.cpp")
INCLUDES = ['"definitions.hpp"']


class Entry(abc.ABC):
    @abc.abstractmethod
    def render(self):
        ...

class Variable(Entry):
    def __init__(self, type_name, name, value):
        self.type_name = type_name
        self.name = name
        self.value = value

    def render(self):
        return f"{self.type_name} {self.name} = {{{self.value}}};"

class String(Entry):
    def __init__(self, name, path):
        self.name = name
        with open(path, encoding="utf-8") as handle:
            self.value = handle.read().rstrip("\n")

    @staticmethod
    def escape(text):
        data = text.encode("utf-8") if isinstance(text, str) else text
        escaped = []
        for byte in data:
            if byte == 0x5C:
                escaped.append("\\\\")
            elif byte == 0x22:
                escaped.append('\\"')
            elif 32 <= byte < 127:
                escaped.append(chr(byte))
            else:
                escaped.append(f"\\{byte:03o}")
        return '"' + "".join(escaped) + '"'

    @staticmethod
    def identifier(filename):
        stem = os.path.splitext(filename)[0]
        name = re.sub(r"[^A-Za-z0-9_]", "", stem)
        if not name or name[0].isdigit():
            name = f"_{name}"
        return name

    def render(self):
        data = Variable("static const char", f"{self.name}[] PROGMEM", self.escape(self.value))
        return f"{data.render()}"

class Dict(Entry):
    def __init__(self, name, path):
        self.name = name
        self.len, self.left, self.right, self.payload = self.pack(self.load(path))

    @staticmethod
    def load(path):
        with open(path, encoding="utf-8") as handle:
            document = json.load(handle)
        if not isinstance(document, dict):
            raise ValueError(f"'{path}' must contain a json object")

        fields = {}
        for key, value in document.items():
            if not isinstance(value, str):
                raise ValueError(f"field '{key}' in '{path}' must be a string")
            fields[key] = value
        return fields

    @staticmethod
    def pack(fields):
        ordered = sorted(fields.items())
        left = max((len(key.encode("utf-8")) for key, _ in ordered), default=0)
        right = max((len(value.encode("utf-8")) for _, value in ordered), default=0)
        payload = bytearray()
        for key, value in ordered:
            payload += key.encode("utf-8").ljust(left, b"\0")
            payload += value.encode("utf-8").ljust(right, b"\0")
        return len(ordered), left, right, bytes(payload)

    def render(self):
        blob = f"{self.name}_blob"
        data = Variable("static const char", f"{blob}[] PROGMEM", String.escape(self.payload))
        dictionary = Variable(
            "Dict",
            f"{self.name}",
            f"{self.left}, {self.right}, {self.len}, static_cast<PGM_P>({blob})",
        )
        return f"{data.render()}\n{dictionary.render()}"

class Scope(Entry):
    def __init__(self, includes=None):
        self.includes = list(includes or [])
        self.entries = []

    def add(self, entry):
        self.entries.append(entry)
        return entry

    def render(self):
        includes = "\n".join(f"#include {header}" for header in self.includes)
        body = "\n".join(entry.render() for entry in self.entries)
        sections = [section for section in (includes, body) if section]
        return "\n\n".join(sections) + "\n"

    def save(self, path):
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w") as handle:
            handle.write(self.render())

def _asset_files(project_dir):
    override_dir = os.path.join(project_dir, OVERRIDE_DIR)
    names = set()
    if os.path.isdir(ASSET_DIR):
        names.update(os.listdir(ASSET_DIR))
    if os.path.isdir(override_dir):
        names.update(os.listdir(override_dir))

    for filename in sorted(names):
        override = os.path.join(override_dir, filename)
        default = os.path.join(ASSET_DIR, filename)
        if os.path.isfile(override):
            yield filename, override
        elif os.path.isfile(default):
            yield filename, default

def generate(project_dir):
    scope = Scope(INCLUDES)
    names = set()
    for filename, path in _asset_files(project_dir):
        extension = os.path.splitext(filename)[1]
        if extension not in (".dict", ".str"):
            continue
        name = String.identifier(filename)
        if name in names:
            raise ValueError(f"'{filename}' maps to duplicate identifier '{name}'")
        names.add(name)
        if extension == ".dict":
            scope.add(Dict(name, path))
        else:
            scope.add(String(name, path))
    scope.save(OUTPUT_FILE)


def pre_script(source, target, env):
    segment = env.get("PROJECT_DIR") or env.subst("$PROJECT_DIR")
    if not segment or segment == "$PROJECT_DIR":
        segment = os.getcwd()
    generate(os.path.abspath(segment))

pre_script(None, None, env)

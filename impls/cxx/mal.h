#pragma once

class MalType;
class Env;

MalType* EVAL(MalType* ast, Env* env);
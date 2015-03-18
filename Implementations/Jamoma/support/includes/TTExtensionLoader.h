#pragma once

// If no path is specified and loadFromBuiltinPaths == false, only try to load from the computed relative path.
// Else if loadFromBuiltinPaths == true, try to load from the builtin paths
// Else if there is a path, and loadFromBuiltinPaths == false, only try to load from the given path
// Else try to load from the given path, and if it does not work, from the computed path,
// and if it still does not work, from the builtin paths.
void TTLoadExtensions(const char* pathToLoadFrom = nullptr, bool loadFromBuiltinPaths = false);

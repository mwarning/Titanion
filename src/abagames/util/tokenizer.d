/*
 * $Id: tokenizer.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2006 Kenta Cho. Some rights reserved.
 */
module abagames.util.tokenizer;

private import std.stdio;
private import std.string;
private import std.conv;

/**
 * Tokenizer.
 */
public class Tokenizer {
 private:

  public static string[] readFile(string fileName, string separator) {
    string[] result;
    File fd = File(fileName);
    for (;;) {
      string line = fd.readln();
      if (!line)
        break;
      string[] spl = std.string.split(line, separator);
      foreach (string s; spl) {
        string r = strip(s);
        if (r.length > 0)
          result ~= r;
      }
    }
    fd.close();
    return result;
  }
}

/**
 * CSV format tokenizer.
 */
public class CSVTokenizer {
 private:

  public static string[] readFile(string fileName) {
    return Tokenizer.readFile(fileName, ",");
  }
}

/*------------------------------------------------------------------------------
 *    Copyright 2015 Chris Rink
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *----------------------------------------------------------------------------*/

#ifndef MSCRIPT_STREAMREADER_H
#define MSCRIPT_STREAMREADER_H

/**
* @file streamreader.h
* @author Chris Rink
* @date 11 Mar 2015
* @brief File and string stream wrapper.
*
* The C standard library provides the FILE object for operating on a
* file stream, but there is no cross platform equivalent to the POSIX
* @c fmemopen stream wrapper for a @c char* . A @c sr_Reader object
* is meant to provide a light wrapper around both object types to
* allow universal access.
*/

#include <stdio.h>

/**
* @brief Stream wrapper object
*
* A @c sr_Reader object is the wrapper for either a @c FILE or a
* @c char* C-style string.
*/
typedef struct sr_Reader sr_Reader;

/**
* @brief Create a new stream from the given string.
*
* @param str the string to iterate on
* @returns a new @c sr_Reader object or NULL if allocation failed
*/
sr_Reader * sr_NewString(const char *str);

/**
* @brief Create a new stream from the given string with input length.
*
* @param str the string to iterate on
* @param len the length of the input string
* @returns a new @c sr_Reader object or NULL if allocation failed
*/
sr_Reader * sr_NewStringL(const char *str, size_t len);

/**
* @brief Create a new stream from the given file.
*
* @param fname the name of a file to open
* @returns a new @c sr_Reader object or NULL if either allocation or
*          opening the specified file failed
*/
sr_Reader * sr_NewFile(const char *fname);

/**
* @brief Destroy a @c sr_Reader object.
*/
void sr_Destroy(sr_Reader *stream);

/**
* @brief Return the next character in the stream.
*
* @param stream the @c sr_Reader object to get the next character from
* @returns the next character from the stream or EOF if there are no
*          more characters in the stream.
*/
int sr_NextChar(sr_Reader *stream);

/**
* @brief Move the stream pointer back one character, allowing that
* character to be read again.
*
* This function uses @c fseek to seek back one character within @c FILE*
* streams, so it should theoretically be able to unread every character
* to the beginning of the stream, unlike @c ungetc which is only
* guaranteed to unread at most one character from the current pointer
* position.
*
* @param stream the @c sr_Reader object to get the next character from
* @returns the previous character in the stream or EOF if the stream is
*          at the beginning
*/
int sr_Unread(sr_Reader *stream);

#endif //MSCRIPT_STREAMREADER_H

/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _DECAF_LANG_EXCEPTIONS_ILLEGALTHREADSTATEEXCEPTION_H_
#define _DECAF_LANG_EXCEPTIONS_ILLEGALTHREADSTATEEXCEPTION_H_

#include <decaf/lang/Exception.h>

namespace decaf {
namespace lang {
namespace exceptions {

    /*
     * Thrown when an error occurs when a user tries to start a Thread that
     * has already been started.
     *
     * @since 1.0
     */
    class DECAF_API IllegalThreadStateException : public Exception {
    public:

        /**
         * Default Constructor
         */
        IllegalThreadStateException();

        /**
         * Conversion Constructor from some other Exception
         *
         * @param ex
         *      The Exception whose data is to be copied into this one.
         */
        IllegalThreadStateException(const Exception& ex);

        /**
         * Copy Constructor
         *
         * @param ex
         *      The Exception whose data is to be copied into this one.
         */
        IllegalThreadStateException(const IllegalThreadStateException& ex);

        /**
         * Constructor - Initializes the file name and line number where
         * this message occured.  Sets the message to report, using an
         * optional list of arguments to parse into the message
         *
         * @param file The file name where exception occurs
         * @param lineNumber The line number where the exception occurred.
         * @param msg The message to report
         * @param ... list of primitives that are formatted into the message
         */
        IllegalThreadStateException(const char* file, const int lineNumber, const char* msg, ...);

        /**
         * Constructor - Initializes the file name and line number where
         * this message occurred.  Sets the message to report, using an
         * optional list of arguments to parse into the message
         *
         * @param file The file name where exception occurs
         * @param lineNumber The line number where the exception occurred.
         * @param cause The exception that was the cause for this one to be thrown.
         * @param msg The message to report
         * @param ... list of primitives that are formatted into the message
         */
        IllegalThreadStateException(const char* file, const int lineNumber, const std::exception* cause, const char* msg, ...);

        /**
         * Constructor
         *
         * @param cause Pointer to the exception that caused this one to
         * be thrown, the object is cloned caller retains ownership.
         */
        IllegalThreadStateException(const std::exception* cause);

        /**
         * Clones this exception.  This is useful for cases where you need
         * to preserve the type of the original exception as well as the message.
         * All subclasses should override.
         *
         * @return an new Exception instance that is a copy of this one.
         */
        virtual IllegalThreadStateException* clone() const {
            return new IllegalThreadStateException(*this);
        }

        virtual ~IllegalThreadStateException() throw();

   };

}}}

#endif /*_DECAF_LANG_EXCEPTIONS_ILLEGALTHREADSTATEEXCEPTION_H_*/
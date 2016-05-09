//////////////////////////////////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright � 2008, Daniel �nnerby
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <core/support/ThreadHelper.h>
#include <core/db/Connection.h>
#include <core/sdk/IMetadataReader.h>

#include <sigslot/sigslot.h>

#include <boost/thread/thread.hpp>

#include <deque>
#include <vector>

namespace musik { namespace core {

    //////////////////////////////////////////
    ///\brief
    ///The Indexer is the class that syncronizes musik tracks with the database
    ///
    ///The Indexer is often a member of classes like the LocalLibrary
    ///but can also be used as a standalone class for indexing files.
    ///All you need to do is create a Indexer object and call Startup()
    //////////////////////////////////////////
    class Indexer : public ThreadHelper, private boost::noncopyable {
        public:
            Indexer();
            ~Indexer();

            void AddPath(std::string sPath);
            void RemovePath(std::string sPath);
            std::vector<std::string> GetPaths();

            bool Startup(std::string setLibraryPath);
            void ThreadLoop();

            std::string GetStatus();
            void RestartSync(bool bNewRestart=true);
            bool Restarted();

            std::string database;

            sigslot::signal0<> SynchronizeStart;
            sigslot::signal0<> SynchronizeEnd;
            sigslot::signal0<> PathsUpdated;
            sigslot::signal0<> TrackRefreshed;

        private:
        
            db::Connection dbConnection;

            std::string libraryPath;
            int status;
            bool restart;

            boost::thread *thread;
            boost::mutex progressMutex;

            double overallProgress;
            double currentProgress;
            int nofFiles;
            int filesIndexed;
            int filesSaved;

            void CountFiles(const std::string &dir);

            void Synchronize();
            void SyncDirectory(const std::string& dir, DBID parentDirId, DBID pathId, std::string &syncPath);
            void SyncDelete(const std::vector<DBID>& paths);
            void SyncCleanup();
            void ProcessAddRemoveQueue();
            void SyncOptimize();
            void RunAnalyzers();

            class AddRemoveContext {
                public:
                    bool add;
                    std::string path;
            };

            typedef std::vector<boost::shared_ptr<metadata::IMetadataReader>> MetadataReaderList;

            std::deque<AddRemoveContext> addRemoveQueue;

            MetadataReaderList metadataReaders;
    };

    typedef boost::shared_ptr<Indexer> IndexerPtr;

} } 
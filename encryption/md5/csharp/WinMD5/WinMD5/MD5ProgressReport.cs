// from http://www.jianshu.com/p/9c456dca8539

using System;
using System.IO;
using System.ComponentModel;

namespace WinMD5
{
    class MD5ProgressReport : Stream, IDisposable
    {
        private Stream stream;
        private BackgroundWorker worker;
        private int alreadyReaded;

        public MD5ProgressReport(Stream s1, BackgroundWorker worker1)
        {
            stream = s1;
            worker = worker1;
            alreadyReaded = 0;
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            int bytesRead = stream.Read(buffer, offset, count);
            alreadyReaded += bytesRead;
            worker.ReportProgress((int)(100 * (float)alreadyReaded / stream.Length));
            return bytesRead;
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            throw new NotImplementedException();
        }

        public override bool CanRead
        {
            get { return stream.CanRead; }
        }

        public override bool CanSeek
        {
            get { return stream.CanSeek; }
        }

        public override bool CanWrite
        {
            get { return stream.CanWrite; }
        }

        public override void Flush()
        {
            stream.Flush();
        }

        public override long Length
        {
            get { return stream.Length; }
        }

        public override long Position
        {
            get
            {
                return stream.Position;
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotImplementedException();
        }

        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        void IDisposable.Dispose()
        {
            stream.Dispose();
        }
    }
}

using System;
using System.ComponentModel;
using System.Threading.Tasks;

namespace Engine.Editor.Commands
{
    sealed class NotifyTaskCompletion : INotifyPropertyChanged
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="innerTask"></param>
        public NotifyTaskCompletion(Task innerTask)
        {
            Task = innerTask;
            if (innerTask.IsCompleted == false)
            {
                TaskCompletion = WatchTaskAsync(innerTask);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="innerTask"></param>
        /// <returns></returns>
        private async Task WatchTaskAsync(Task innerTask)
        {
            try { await innerTask; }
            catch { }

            if (PropertyChanged == null) return;

            PropertyChanged(this, new PropertyChangedEventArgs("Status"));
            PropertyChanged(this, new PropertyChangedEventArgs("IsCompleted"));
            PropertyChanged(this, new PropertyChangedEventArgs("IsNotCompleted"));

            if (innerTask.IsCanceled)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("IsCanceled"));
            }
            else if (innerTask.IsFaulted)
            {
                PropertyChanged(this, new PropertyChangedEventArgs("IsFaulted"));
                PropertyChanged(this, new PropertyChangedEventArgs("Exception"));
                PropertyChanged(this, new PropertyChangedEventArgs("InnerException"));
                PropertyChanged(this, new PropertyChangedEventArgs("ErrorMessage"));
            }
            else
            {
                PropertyChanged(this, new PropertyChangedEventArgs("IsSuccessfullyCompleted"));
            }
        }

        /// <summary>
        /// 
        /// </summary>
        public Task Task { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public Task TaskCompletion { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        public TaskStatus Status { get { return Task.Status; } }

        /// <summary>
        /// 
        /// </summary>
        public bool IsCompleted { get { return Task.IsCompleted; } }

        /// <summary>
        /// 
        /// </summary>
        public bool IsNotCompleted { get { return Task.IsCompleted == false; } }

        /// <summary>
        /// 
        /// </summary>
        public bool IsSuccessfullyCompleted
        {
            get { return Task.Status == TaskStatus.RanToCompletion; }
        }

        /// <summary>
        /// 
        /// </summary>
        public bool IsCanceled { get { return Task.IsCanceled; } }

        /// <summary>
        /// 
        /// </summary>
        public bool IsFaulted { get { return Task.IsFaulted; } }

        /// <summary>
        /// 
        /// </summary>
        public AggregateException Exception { get { return Task.Exception; } }

        /// <summary>
        /// 
        /// </summary>
        public Exception InnerException { get { return Exception?.InnerException; } }

        /// <summary>
        /// 
        /// </summary>
        public string ErrorMessage { get { return InnerException?.Message; } }

        /// <summary>
        /// 
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;
    }
}

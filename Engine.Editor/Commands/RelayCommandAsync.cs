using System;
using System.Threading.Tasks;

namespace Engine.Editor.Commands
{
    class RelayCommandAsync : AbstractCommandAsync
    {
        private readonly Func<object, Task> _command;

        /// <summary>
        /// 
        /// </summary>
        protected RelayCommandAsync()
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="command"></param>
        public RelayCommandAsync(Func<object, Task> command)
        {
            _command = command;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        public override bool CanExecute(object parameter) => Execution == null || Execution.IsCompleted;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        public override async Task ExecuteAsync(object parameter)
        {
            if (_command == null) return;

            Execution = new NotifyTaskCompletion(_command(parameter));
            RaiseCanExecuteChanged();
            await Execution.TaskCompletion;
            RaiseCanExecuteChanged();
        }

        /// <summary>
        /// 
        /// </summary>
        public NotifyTaskCompletion Execution { get; protected set; }
    }
}

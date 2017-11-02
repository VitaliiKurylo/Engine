using System;
using System.Threading.Tasks;

namespace Engine.Editor.Commands
{
    class RelayCommandAsync<TResult> : RelayCommandAsync
    {
        private readonly Func<object, Task<TResult>> _command;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="command"></param>
        public RelayCommandAsync(Func<object, Task<TResult>> command)
        {
            _command = command;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        public override async Task ExecuteAsync(object parameter)
        {
            if (_command == null) return;

            Execution = new NotifyTaskCompletion<TResult>(_command(parameter));
            RaiseCanExecuteChanged();
            await Execution.TaskCompletion;
            RaiseCanExecuteChanged();
        }

        /// <summary>
        /// 
        /// </summary>
        public new NotifyTaskCompletion<TResult> Execution { get; protected set; }
    }
}

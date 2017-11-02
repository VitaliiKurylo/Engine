using System;
using System.Windows.Input;

namespace Engine.Editor.Commands
{
    class RelayCommand : ICommand
    {
        private readonly Action<object> _command;
        private readonly Predicate<object> _canExecutePredicate;

        /// <summary>
        /// 
        /// </summary>
        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        public bool CanExecute(object parameter) => _canExecutePredicate == null || _canExecutePredicate(parameter);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        public void Execute(object parameter)
        {
            _command(parameter);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="command"></param>
        public RelayCommand(Action<object> command) : this(command, null)
        {
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="command"></param>
        /// <param name="canExecutePredicate"></param>
        public RelayCommand(Action<object> command, Predicate<object> canExecutePredicate)
        {
            _command = command;
            _canExecutePredicate = canExecutePredicate;
        }
    }
}

using System.Threading.Tasks;
using System.Windows.Input;

namespace Engine.Editor.Commands
{
    interface ICommandAsync : ICommand
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        Task ExecuteAsync(object parameter);
    }
}

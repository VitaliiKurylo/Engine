using Engine.Editor.Attributes;
using Engine.Editor.ViewModels;
using Engine.Editor.ViewModels.ToolBar;
using System;
using System.Linq;
using System.Windows.Input;

namespace Engine.Editor.Binding
{
    class ToolBarBinding
    {
        private AbstractViewModel _source;

        /// <summary>
        /// 
        /// </summary>
        public ToolBarBinding() : this(null)
        {

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="source"></param>
        public ToolBarBinding(AbstractViewModel source)
        {
            _source = source;
        }

        /// <summary>
        /// 
        /// </summary>
        public ToolBarViewModel GetBinding()
        {
            var toolBarViewModel = default(ToolBarViewModel);

            if (_source == null) return toolBarViewModel;

            var sourceType = _source.GetType();
            var toolBarAttribute = Attribute.GetCustomAttribute(sourceType, typeof(ToolBarAttribute)) as ToolBarAttribute;
            if (toolBarAttribute == null) return toolBarViewModel;
            else
            {
                toolBarViewModel = new ToolBarViewModel { ElementTemplateSelector = toolBarAttribute.ElementTemplateSelector };
            }

            var toolBarButtonAttribute = default(ToolBarButtonAttribute);
            foreach (var propertyInfo in sourceType.GetProperties().Where(property => Attribute.IsDefined(property, typeof(ToolBarButtonAttribute))))
            {
                toolBarButtonAttribute = (ToolBarButtonAttribute)Attribute.GetCustomAttribute(propertyInfo, typeof(ToolBarButtonAttribute));
                toolBarViewModel.Model.Add(new ToolBarButtonViewModel
                {
                    Content = toolBarButtonAttribute.Content,
                    Command = (ICommand)propertyInfo.GetValue(_source)
                });
            }

            return toolBarViewModel;
        }
    }
}

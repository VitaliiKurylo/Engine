using System;
using System.Windows;

namespace Engine.Editor.Attributes
{
    [AttributeUsage(AttributeTargets.Class)]
    class ToolBarAttribute : Attribute
    {
        public Type ElementTemplateSelector { get; private set; }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="templateSelector"></param>
        public ToolBarAttribute(Type templateSelector)
        {
            ElementTemplateSelector = templateSelector;
        }
    }
}

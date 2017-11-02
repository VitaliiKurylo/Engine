using System;

namespace Engine.Editor.Attributes
{
    [AttributeUsage(AttributeTargets.Property)]
    class ToolBarButtonAttribute : Attribute
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="content"></param>
        public ToolBarButtonAttribute(string content)
        {
            Content = content;
        }

        /// <summary>
        /// 
        /// </summary>
        public string Content { get; private set; }
    }
}

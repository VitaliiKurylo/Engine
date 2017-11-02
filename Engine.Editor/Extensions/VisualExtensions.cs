using System.IO;
using System.Reflection;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Engine.Editor.Extensions
{
    static class VisualExtensions
    {
        public static void ToBitmapFile(this Visual visual, string filePath)
        {
            // Encode
            var encoder = new PngBitmapEncoder();
            encoder.Frames.Add(BitmapFrame.Create(visual.ToBitmapSource()));

            using (var stream = File.Create(filePath))
            {
                encoder.Save(stream);
            }
        }
        
        public static BitmapSource ToBitmapSource(this Visual visual)
        {
            // Get height and width
            var height = (int)(double)visual.GetValue(FrameworkElement.ActualWidthProperty);
            var width = (int)(double)visual.GetValue(FrameworkElement.ActualHeightProperty);

            width = width == 0 ? 1 : width;
            height = height == 0 ? 1 : height;

            var dpiXProperty = typeof(SystemParameters).GetProperty("DpiX", BindingFlags.NonPublic | BindingFlags.Static);
            var dpiYProperty = typeof(SystemParameters).GetProperty("Dpi", BindingFlags.NonPublic | BindingFlags.Static);

            var dpiX = (int)dpiXProperty.GetValue(null, null);
            var dpiY = (int)dpiYProperty.GetValue(null, null);

            // Render
            var renderTargetBitmap = new RenderTargetBitmap(height, width, dpiX, dpiY, PixelFormats.Default);
            renderTargetBitmap.Render(visual);

            return renderTargetBitmap;
        }
        
        public static TChild GetVisualChild<TChild>(this Visual parent) where TChild : Visual
        {
            var child = default(TChild);

            var childrenCount = VisualTreeHelper.GetChildrenCount(parent);
            for (var index = 0; index < childrenCount; index++)
            {
                var visual = (Visual)VisualTreeHelper.GetChild(parent, index);
                child = visual as TChild;
                if (child == null)
                {
                    child = GetVisualChild<TChild>(visual);
                }
                else
                {
                    break;
                }
            }

            return child;
        }
        
        public static TParent GetParent<TParent>(this Visual child) where TParent : Visual
        {
            while (true)
            {
                var parent = (Visual)VisualTreeHelper.GetParent(child);
                if (parent.GetType() == typeof(TParent) || parent.GetType().IsSubclassOf(typeof(TParent)))
                    return (TParent)parent;

                child = parent;
            }
        }
    }
}

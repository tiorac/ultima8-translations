using Google.Cloud.Translation.V2;

namespace TioRAC.U8GT
{
    internal class TranslationProcess
    {
        internal TranslationProcess(ProgramOptions options)
        {
            Options = options;
        }

        private readonly ProgramOptions Options;

        private TranslationClient? _translation;
        internal TranslationClient Translation
        {
            get
            {
                if (_translation == null)
                {
                    if (!string.IsNullOrWhiteSpace(Options.ApiKey))
                        _translation = TranslationClient.CreateFromApiKey(Options.ApiKey);
                    else
                        _translation = TranslationClient.Create();
                }

                return _translation;
            }
        }

        internal async Task ProcessAsync()
        {
            string originFolderPath = Options.OriginFolderPath;
            string destinyFolderPath = Options.DestinyFolderPath;

            Console.WriteLine($"Origin folder path: {originFolderPath}");
            Console.WriteLine($"Destiny folder path: {destinyFolderPath}");

            string[] txtFiles = Directory.GetFiles(originFolderPath, "*.txt");
            int totalCharacters = 0;

            foreach (string txtFile in txtFiles)
            {
                var fileName = Path.GetFileName(txtFile);
                Console.WriteLine($"Processing file {fileName}...");

                string[] lines = File.ReadAllLines(txtFile);

                for (int i = 0; i < lines.Length; i++)
                {
                    string originalText = lines[i];

                    int totalPipes = originalText.Count(c => c == '|');

                    if (totalPipes < 3)
                        continue;

                    string[] parts = originalText.Split('|');
                    string textToTranslate = parts[3].Trim();

                    if (string.IsNullOrWhiteSpace(textToTranslate))
                        continue;

                    totalCharacters += textToTranslate.Length;

                    string translatedText = await TranslateTextAsync(textToTranslate);
                    translatedText = ReplaceAccentuation(translatedText);

                    lines[i] = originalText.Replace(textToTranslate, translatedText);
                    Console.WriteLine($"Total characters: {totalCharacters}");
                }

                if (!Directory.Exists(destinyFolderPath))
                    Directory.CreateDirectory(destinyFolderPath);

                string destinationFilePath = Path.Combine(destinyFolderPath, Path.GetFileName(txtFile));
                File.WriteAllLines(destinationFilePath, lines);
                Console.WriteLine($"Total characters: {totalCharacters}");
            }

            Console.WriteLine("Process finished!");
        }

        private async Task<string> TranslateTextAsync(string text)
        {
            if (Options.Language == null)
                return text;

            TranslationResult result = await Translation.TranslateTextAsync(
                               text, Options.Language);

            return result.TranslatedText;

            //return text.ToUpper();
        }

        private string ReplaceAccentuation(string text)
        {
            if (!Options.Accentuation)
                return text;

            var table = AccentsTablePt.Table;
            string newText = text;

            foreach (var item in table)
                newText = newText.Replace(item.Key, (char)item.Value);

            return newText;
        }
    }
}

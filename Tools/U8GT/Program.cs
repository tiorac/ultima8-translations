using CommandLine;

namespace TioRAC.U8GT
{
    class Program
    {
        static void Main(string[] args)
        {
            Parser.Default.ParseArguments<ProgramOptions>(args)
                .WithParsed(options =>
                {
                    if (options.Language != null
                        && string.IsNullOrWhiteSpace(options.ApiKey)
                        && Environment.GetEnvironmentVariable("GOOGLE_APPLICATION_CREDENTIALS") == null)
                    {
                        Console.Write("Google Cloud Translation API Key: ");
                        options.ApiKey = Console.ReadLine();
                    }

                    TranslationProcess process = new(options);
                    process.ProcessAsync().Wait();
                });
        }
    }
}
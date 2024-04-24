using CommandLine;

namespace TioRAC.U8GT
{
    internal class ProgramOptions
    {
        [Option('o', "origin", Required = false, Default = ".", HelpText = "Origin folder path")]
        public string OriginFolderPath { get; set; } = ".";

        [Option('d', "destiny", Required = false, Default = "./translation", HelpText = "Destiny folder path")]
        public string DestinyFolderPath { get; set; } = "./translation";

        [Option('l', "language", Required = false, Default = null, HelpText = "Language to translate! If null, do not carry out the translation process.")]
        public string? Language { get; set; } = null;

        [Option('a', "accentuation", Required = false, Default = false, HelpText = "If true, replaces the accent bytes.")]
        public bool Accentuation { get; set; } = true;

        [Option('k', "api-key", Required = false, Default = null, HelpText = "Google Cloud Translation API Key. If null, the environment variable GOOGLE_APPLICATION_CREDENTIALS will be used.")]
        public string? ApiKey { get; set; } = null;
    }
}

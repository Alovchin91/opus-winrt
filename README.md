# Opus Codec for Windows Runtime

**opus-winrt** is a work in progress for a simple Windows Runtime component for decoding (so far) [Opus](http://opus-codec.org/) files. It is being written completely in C++/CX, and is based on original _opus v1.1_ and _opusfile v0.6_ C libraries source code. It is available for C# and C++/CX developers writing Windows Store apps for Windows (Phone) 8.1 and higher.

Opus Codec for Windows Runtime currently only exposes a part of decoding functionality found in original _opusfile_ library. If you think encoder and low-level Opus API access are necessary, feel free to contribute ;) Still, as far as Windows Runtime matures and becomes a technology that can be used in desktop development as well, Opus Codec for Windows Runtime will also be updated and necessary functionality will be added.

## Sample usage

**opus-winrt**'s API is similar to original [Opusfile API](http://opus-codec.org/docs/opusfile_api-0.6/index.html), except that it's object-oriented. The main class that you will use to decode an Opus file stream is `Opusfile.WindowsRuntime.OggOpusFile` class. Instantiate the class and then call `OggOpusFile.Open` to initialize the stream decoder. To get samples from the decoder, simply use `OggOpusFile.Read` method.

**Coming soon**: Example solution containing sample Windows Phone 8.1 background audio project that uses Opus Codec for Windows Runtime.

Here's the sample usage. Assume you have a managed helper class with `OggOpusFile _opusFile` field that is supposed to help you wrap all the decoder logic. Here's how you initialize the decoder:

```cs
public void Initialize(IRandomAccessStream fileStream)
{
    this._opusFile.Open(fileStream);
    
    if (!this._opusFile.IsValid)
        throw new InvalidOperationException();
}
```

Note that if something goes wrong, the `OggOpusFile.Open()` method will throw an exception with HResult equal to one of opusfile's [error codes](http://opus-codec.org/docs/opusfile_api-0.6/group__error__codes.html), but still you should check for `OggOpusFile.IsValid` each time you call any `OggOpusFile` API.

Now, to get a sample from the decoder, you can write a simple method like this:

```cs
public IBuffer GetSample(int length)
{
    if (!this._opusFile.IsValid)
        throw new InvalidOperationException();
    
    return this._opusFile.Read(length);
}
```

## How to build

**opus-winrt** includes all the necessary source code to build the libraries. Opus Codec for Windows Runtime solution includes original Opus libraries and their dependencies, including [libogg](http://downloads.xiph.org/releases/ogg/), and contains opusfile_winrt project that is the main output of the solution.

You will need Visual Studio 2013 or higher to build the library for Windows (Phone) 8.1 or higher. All the projects currently are set up to build for Windows Phone 8.1, but it's easy to retarget them for Windows 8.1 or higher.


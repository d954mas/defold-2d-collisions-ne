# {"version": "1.2.89", "sha1": "5ca3dd134cc960c35ecefe12f6dc81a48f212d40"}
# Get SHA1 of the current Defold stable release
echo "Try get sha"
SHA1=$(curl -s http://d.defold.com/stable/info.json | sed 's/.*sha1": "\(.*\)".*/\1/')
echo "Using Defold ${SHA1}"
BOB_URL="http://d.defold.com/archive/${SHA1}/bob/bob.jar"
# Download bob.jar
echo "Downloading ${BOB_URL}"
curl -o bob.jar ${BOB_URL}
yes | cp -rf ./release.appmanifest ./manifest.appmanifest
yes | cp -rf ./default.texture_profiles ./release.texture_profiles
mkdir bundles
java -jar bob.jar -d -build-report-html bundles/report.html --email foo@bar.ru --auth 123456 --archive  resolve distclean build
java -jar bob.jar -d --platform x86-win32 --archive build
java -jar bob.jar -d --platform x86-win32 --bundle-output bundles/win32  bundle
java -jar bob.jar -d --platform x86-darwin --archive build
java -jar bob.jar -d --platform x86-darwin --bundle-output bundles/macOSX bundle
java -jar bob.jar -d --platform armv7-android --archive build
java -jar bob.jar -d --platform armv7-android --bundle-output bundles/android bundle
java -jar bob.jar -d --platform js-web --archive build
java -jar bob.jar -d --platform js-web --bundle-output bundles/web bundle
tar -zcvf all_bundles.tar.gz bundles
tar -zcvf win32.tar.gz bundles/win32
tar -zcvf web.tar.gz bundles/web
tar -zcvf macOSX.tar.gz bundles/macOSX
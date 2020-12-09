const { Console } = require('console');
const util = require('util');
const exec = util.promisify(require('child_process').exec);
const getAppDataPath = require("appdata-path");

var WebSocketServer = require('ws').Server
var wss = new WebSocketServer({port: 1337});

var norun = 0;
var success = 0;
var fail = 0;
var failed_tests = [];
var total_tests_count = 0;

var assert_failed = 0;
var assert_success = 0;
var current_patcher = '';
var prefs_rawdata;
var overdrive = 0;

var max_prefs_file = "";

wss.on('connection', function(ws) {
    ws.on('message', function(message) {
        var json = JSON.parse(message);
        switch(json.operation)
        {
            case 'assert':
            {
                switch(json.status)
                {
                    case 'norun':
                        norun++;
                        break;
                    case 'fail':
                        fail++;
                        assert_failed++;
                        failed_tests.push(current_patcher);
                        break;
                    case 'success':
                        assert_success++;
                        success++;
                        break;

                }
                console.log("\t> Asert: " + json.status + " - " + json.name);
                break;
            }
            case 'initWatchdog':
            {
                console.log('Using libossia version ' + json.version);
                break;
            }
            default:
            {
                console.log('received: %s', message);
            }
        }
    });
});

wss.on('close', function close() {
    console.log("open next patcher");
});

//requiring path and fs modules
const path = require('path');
const fs = require('fs');
const { report, exit } = require('process');

async function main()
{ 
    directory_path = __dirname;
    console.log('Directory path: ' + directory_path);
    max_prefs_file = path.join(getAppDataPath(), "Cycling '74/Max 8/Settings/maxpreferences.maxpref");

    read_max_prefs();

    args = process.argv.slice(2);
    if(args.length > 0)
    {
        files = args;
    }
    else
    {
        directory_path = path.join(__dirname, 'test-patchers');
        files = fs.readdirSync(directory_path);    
    }

    for(overdrive = 0; overdrive<2; overdrive++)
    {
        for(let index = 0; index < files.length; index++)
        {
            file = files[index];
            if(file.endsWith(".ossia-max-test.maxpat"))
            {
                console.log("Open " + file);
                total_tests_count++;
                patcher_path = path.join(directory_path, file);
                current_patcher = file;
                assert_failed = 0;
                assert_success = 0;
                await exec('open -W -n ' + patcher_path);
                if(assert_failed + assert_success == 0)
                {
                    failed_tests.add(current_patcher);
                }
                console.log("");
            }
        }
        report_and_exit();
    }
   
    restore_max_prefs();
    process.exit(failed_tests.size);

}

function launch_max(patcher_path)
{
    console.log("Open " + patcher_path);
    const process = spawn("open", ['-W', '-n', patcher_path]);

    process.on('close', (code) => {
        console.log(`child process exited with code ${code}`);
    });

    process.on('error', (err) => {
        console.error('Failed to start Max subprocess.');
        process.exit(-1);
    });
}

function report_and_exit()
{
    console.log("\n\n");
    console.log('======');
    console.log('Report');
    console.log('======\n');
    console.log('Overdrive: ' + overdrive);
    console.log("Assertions:");
    console.log("Total\tSuccess\tFail\tNorun");
    var total = success + fail + norun;
    console.log(total + "\t" 
    + success + "(" + 100 * success  / total + "%)\t" 
    + fail + "(" + 100 * fail  / total + "%)\t"
     + norun + "(" + 100 * norun  / total + "%)");
    
    console.log("Tests:");
    console.log("Total\tFailed");
    console.log(total_tests_count + "\t" + failed_tests.length + " (" + 100 * failed_tests.length / total_tests_count + "%)");
    console.log("\nList of failed tests:");
    failed_tests.forEach(function(file) {
        console.log(file);
    });
    console.log();
}

function read_max_prefs()
{
    prefs_rawdata = fs.readFileSync(max_prefs_file);
    prefs = JSON.parse(prefs_rawdata);

    // disable window restoratin on load
    prefs["preferences"]["restorewindows"] = 0;
    write_max_prefs();
}

function write_max_prefs()
{
    var data = JSON.stringify(prefs);
    fs.writeFileSync(max_prefs_file, data);
}

function restore_max_prefs()
{
    fs.writeFileSync(max_prefs_file, prefs_rawdata);
}

main();
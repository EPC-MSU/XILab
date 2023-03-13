pipeline {
  agent { label "master" }

  parameters {
    choice(name: 'XIMC_RELEASE_TYPE', choices: ['RELEASE', 'DEBUG'], description: 'Build and sign type')
  }

  triggers {
    // enable weekly rebuilds
    cron('H H(6-9) * * 1')
  }

  options {
    disableConcurrentBuilds()
    parallelsAlwaysFailFast()
  }

  environment {
    // evaluates to null if no XIMC_VER defined
    LIBXIMC_VERSION = sh(returnStdout: true, script: "sed '/XIMC_VER/!d;s/XIMC_VER=//' VERSIONS").trim()
    XIMC_RELEASE_TYPE = "${params.XIMC_RELEASE_TYPE}"
  }

  stages {

    stage('init') {
      // execute at master
      steps {
        echo "Libximc version=${LIBXIMC_VERSION}"
      }
    } // stage

    stage('build') {
      when {
        expression { env.LIBXIMC_VERSION != null }
      }
      matrix {
        // limit execution at agent with matching label
        agent {
          label "${env.BUILDOS}"
        }
        axes {
          axis {
            name 'BUILDOS'
            values 'debian64', 'debian32', 'win', 'osx'
          }
        }
        stages {

          stage('prebuild') {
            steps {
              echo "Pre"
              echo "Building at BUILDOS=${BUILDOS}"
            }
          } // stage

          stage('build-unix') {
            // Build for Linux and OSX agents
            when { expression { isUnix() } }
            steps {
              // Just to be sure delete any local leftover files
              sh "rm -fv *ximc*.tar.gz"
              // Fetch libximc artifact, any local filename is okay
              sh "curl -Ss -o ximc-0.0.tar.gz https://artifacts.ci.ximc.ru/jenkins/libximc/libximc-${LIBXIMC_VERSION}-all.tar.gz"
              sh "./build.sh add_service_build"
              //touch file: "xilab-fake-${BUILDOS}.tar.gz"
              sh "ls"
              stash name: "result-${BUILDOS}", includes: "mdrive_direct_control-*.AppImage,mdrive_direct_control-*.tar.gz,profile-*.tar.gz"
            }
          } // stage

          stage('build-win') {
            // Build for Windows agents
            when { expression { !isUnix() } }
            steps {
              // Just to be sure delete any local leftover files
              powershell "Remove-Item *ximc*.tar.gz"
              // Fetch libximc artifact, any local filename is okay
              powershell "Invoke-WebRequest -Uri https://artifacts.ci.ximc.ru/jenkins/libximc/libximc-${LIBXIMC_VERSION}-all.tar.gz -OutFile ximc-0.0.tar.gz"
              bat "build.bat add_service_build"
              //touch file: "xilab-fake-${BUILDOS}.exe"
              stash name: "result-${BUILDOS}", includes: "mdrive_direct_control-*.exe, mdrive_direct_control-*.7z"
            }
          } // stage
        } // stages
        post {
          cleanup {
            // drop workspace for each matrix cell job
            deleteDir()
          }
        }
      } // matrix
    } // stage

    stage('pack') {
      when {
        expression { env.LIBXIMC_VERSION != null }
      }
      // execute on master
      steps {
        // Get all stashed archives
        unstash "result-debian64"
        unstash "result-debian32"
        unstash "result-win"
        unstash "result-osx"
        sh "ls"
        sh "7z a -mx0 mdrive_direct_control-release_${BRANCH_NAME}_build-${BUILD_ID}.7z mdrive_direct_control-*.tar.gz mdrive_direct_control-win32.7z mdrive_direct_control-win64.7z mdrive_direct_control-*.exe mdrive_direct_control-*.AppImage"
        sh "7z l mdrive_direct_control-release_${BRANCH_NAME}_build-${BUILD_ID}.7z"
        archiveArtifacts artifacts: "mdrive_direct_control-release*7z,profile-*.tar.gz"
      }
    } // stage

  } // stages

  post {
    failure {
      echo "Failure, sending emails..."
      emailext body: '$DEFAULT_CONTENT',
               to: '$DEFAULT_RECIPIENTS',
               recipientProviders: [[$class: 'DevelopersRecipientProvider'],[$class: 'CulpritsRecipientProvider']],
               subject: '$DEFAULT_SUBJECT'
    }
    cleanup {
      // drop workspace for main job
      deleteDir()
    }
  }
}

